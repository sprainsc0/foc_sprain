#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "debug.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "flashfs.h"
#include "misc/crc32.h"
#include "foc_function.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef uint32_t h_magic_t;
typedef uint16_t h_size_t;
typedef uint16_t h_flag_t;
typedef uint16_t data_size_t;

typedef enum  flash_config_t {
	LargestBlock = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

typedef enum  flash_flags_t {
	SizeMask      = 0x0003,
	MaskEntry     = ~SizeMask,
	BlankEntry    = (h_flag_t)BlankSig,
	ValidEntry    = (0xa5ac & ~SizeMask),
	ErasedEntry   = 0x0000,
} flash_flags_t;


/* File flash_entry_header_t will be sizeof(h_magic_t) aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * sizeof(h_magic_t)
 *  The
 */
typedef struct flash_entry_header_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to erase this entry */
	uint32_t             crc;             /* Calculated over the size - end of data */
	h_size_t             size;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} flash_entry_header_t;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
static uint8_t *working_buffer;
static uint16_t working_buffer_size;
static bool working_buffer_static;
static sector_descriptor_t *sector_map;
static int last_erased;

/****************************************************************************
 * Public Data
 ****************************************************************************/

const flash_file_token_t parameters_token = {
	.n = {'p', 'a', 'r', 'm'},
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/


static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else
  {
    sector = FLASH_SECTOR_11;  
  }
 
  return sector;
}

int16_t up_progmem_write(uint32_t addr, const void *buf, uint32_t count)
{
    uint16_t *hword = (uint16_t *)buf;

    if (count & 1) {
        return -1;
    }

	if (addr >= 0x08000000) {
      	addr -= 0x08000000;
    }
    
    HAL_FLASH_Unlock();

    for(addr += 0x08000000; count; count -= 2, hword++, addr += 2) {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (uint32_t)(addr), *(uint32_t*)(hword)) == HAL_OK) {
            if((*(volatile uint16_t *)(addr)) != *hword)
            {
                /* Flash content doesn't match SRAM content */
                return -1;
            }
        } else {
            return -1;
        }
    }

	HAL_FLASH_Lock();
    return count;
}
/****************************************************************************
 * Name: parameter_flashfs_free
 *
 * Description:
 *   Frees  dynamically allocated memory
 *
 *
 ****************************************************************************/

void parameter_flashfs_free(void)
{
	if (!working_buffer_static && working_buffer != NULL) {
		vPortFree(working_buffer);
		working_buffer = NULL;
		working_buffer_size = 0;
	}
}

/****************************************************************************
 * Name: blank_flash
 *
 * Description:
 *   This helper function returns true if the pointer points to a blank_flash
 *   location
 *
 * Input Parameters:
 *   pf   - A pointer to memory aligned on sizeof(uint32_t) boundaries
 *
 * Returned value:
 *   true if blank
 *
 *
 ****************************************************************************/

static inline int blank_flash(uint32_t *pf)
{
	return *pf == BlankSig;
}

/****************************************************************************
 * Name: blank_check
 *
 * Description:
 *   Given a pointer to a flash entry header and a new size
 *
 * Input Parameters:
 *   pf       - A pointer to the current flash entry header
 *   new_size - The total number of bytes to be written
 *
 * Returned value:
 *  true if space is blank, If it is not blank it returns false
 *
 ****************************************************************************/

static bool blank_check(flash_entry_header_t *pf,
			size_t new_size)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_size /= sizeof(uint32_t);

	while (new_size-- && rv) {
		if (!blank_flash(pm++)) {
			rv = false;
		}
	}

	return rv;
}

/****************************************************************************
 * Name: valid_magic
 *
 * Description:
 *   This helper function returns true if the pointer points to a valid
 *   magic signature
 *
 * Input Parameters:
 *   pm   - A pointer to memory aligned on sizeof(h_magic_t) boundaries
 *
 * Returned value:
 *   true if magic is valid
 *
 *
 ****************************************************************************/

static inline int valid_magic(h_magic_t *pm)
{
	return *pm == MagicSig;
}

/****************************************************************************
 * Name: blank_magic
 *
 * Description:
 *   This helper function returns true if the pointer points to a valid
 *   blank magic signature
 *
 * Input Parameters:
 *   pm   - A pointer to memory aligned on sizeof(h_magic_t) boundaries
 *
 * Returned value:
 *   true if magic is valid
 *
 *
 ****************************************************************************/

static inline int blank_magic(h_magic_t *pm)
{
	return *pm == BlankSig;
}
/****************************************************************************
 * Name: erased_entry
 *
 * Description:
 *   This helper function returns true if the entry is Erased
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if  erased
 *
 *
 ****************************************************************************/

static inline int erased_entry(flash_entry_header_t *fi)
{
	return (fi->flag & MaskEntry) == ErasedEntry;
}

/****************************************************************************
 * Name: blank_entry
 *
 * Description:
 *   This helper function returns true if the entry is Blank
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if Blank
 *
 *
 ****************************************************************************/

static inline int blank_entry(flash_entry_header_t *fi)
{
	return fi->magic == BlankSig  &&  fi->flag == BlankEntry;
}

/****************************************************************************
 * Name: valid_entry
 *
 * Description:
 *   This helper function returns true if the entry is Blank
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if valid_entry
 *
 *
 ****************************************************************************/

static inline int valid_entry(flash_entry_header_t *fi)
{
	return (fi->flag & MaskEntry) == ValidEntry;
}

/****************************************************************************
 * Name: entry_size_adjust
 *
 * Description:
 *   This helper function returns the size adjust
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if valid_entry
 *
 *
 ****************************************************************************/

static inline int entry_size_adjust(flash_entry_header_t *fi)
{
	return fi->flag & SizeMask;
}

/****************************************************************************
 * Name: next_entry
 *
 * Description:
 *   This helper function advances the flash entry header pointer to the
 *   locations of the next entry.
 *
 * Input Parameters:
 *   fh            - A pointer to the current file header
 *
 * Returned value:
 *                - A pointer to the next file header location
 *
 *
 ****************************************************************************/

static inline flash_entry_header_t *next_entry(flash_entry_header_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	return (flash_entry_header_t *) &pb[fi->size];
}

/****************************************************************************
 * Name: entry_data
 *
 * Description:
 *   This helper function returns a pointer the the data in the entry
 *
 * Input Parameters:
 *   fh            - A pointer to the current file header
 *
 * Returned value:
 *                - A pointer to the next file header location
 *
 *
 ****************************************************************************/

static inline uint8_t *entry_data(flash_entry_header_t *fi)
{
	return ((uint8_t *)fi) + sizeof(flash_entry_header_t);
}

/****************************************************************************
 * Name: entry_data_length
 *
 * Description:
 *   This helper function returns the size of the user data
 *
 * Input Parameters:
 *   fh            - A pointer to the current file header
 *
 * Returned value:
 *                - The length of the data in the entry
 *
 *
 ****************************************************************************/

static inline data_size_t entry_data_length(flash_entry_header_t *fi)
{
	return fi->size - (sizeof(flash_entry_header_t) + entry_size_adjust(fi));
}

/****************************************************************************
 * Name: entry_crc_start
 *
 * Description:
 *   This helper function returns a const byte pointer to the location
 *   where the CRC is calculated over
 *
 * Input Parameters:
 *   fi  - A pointer to the current file header
 *
 * Returned value:
 * A pointer to the point at which the crc is calculated from.
 *
 *
 ****************************************************************************/

static inline const uint8_t *entry_crc_start(flash_entry_header_t *fi)
{
	return (const uint8_t *)&fi->size;
}

/****************************************************************************
 * Name: entry_crc_length
 *
 * Description:
 *   This helper function returns the length of the regone where the CRC is
 *   calculated over
 *
 * Input Parameters:
 *   fi  - A pointer to the current file header
 *
 * Returned value:
 * Number of bytes to to crc
 *
 *
 ****************************************************************************/

static inline data_size_t entry_crc_length(flash_entry_header_t *fi)
{
	return fi->size - offsetof(flash_entry_header_t, size);
}

/****************************************************************************
 * Name: find_entry
 *
 * Description:
 *   This helper function locates an "file" from the the file token
 *
 * Input Parameters:
 *   token        - A flash file token, the pseudo file name
 *
 * Returned value:
 *  On Success a pointer to flash entry header or NULL on failure
 *
 *
 ****************************************************************************/

static flash_entry_header_t *find_entry(flash_file_token_t token)
{
	for (int s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / sizeof(h_magic_t)) - 1;

		/* Hunt for Magic Signature */
cont:

		while (pmagic != pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		/* Did we reach the end
		 * if so try the next sector */

		if (pmagic == pe) { continue; }

		/* Found a magic So assume it is a file header */

		flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

		/* Test the CRC */

		if (pf->crc == crc32(entry_crc_start(pf), entry_crc_length(pf))) {

			/* Good CRC is it the one we are looking for ?*/

			if (valid_entry(pf) && pf->file_token.t == token.t) {

				return pf;

			} else {

				/* Not the one we wanted but we can trust the size */

				pf = next_entry(pf);
				pmagic = (h_magic_t *) pf;

				/* If the next one is erased */

				if (blank_entry(pf)) {
					continue;
				}
			}

			goto cont;

		} else {

			/* in valid CRC so keep looking */

			pmagic++;
		}
	}

	return NULL;
}

/****************************************************************************
 * Name: find_free
 *
 * Description:
 *   This helper function locates free space for the number of bytes required
 *
 * Input Parameters:
 *   required  - Number of bytes required
 *
 * Returned value:
 *  On Success a pointer to flash entry header or NULL on failure
 *
 *
 ****************************************************************************/

static flash_entry_header_t *find_free(data_size_t required)
{
	for (int s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / sizeof(h_magic_t)) - 1;

		/* Hunt for Magic Signature */

		do {

			if (valid_magic(pmagic)) {

				flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

				/* Test the CRC */

				if (pf->crc == crc32(entry_crc_start(pf), entry_crc_length(pf))) {

					/* Valid Magic and CRC look for the next record*/

					pmagic = ((uint32_t *) next_entry(pf));

				} else {

					pmagic++;
				}
			}

			if (blank_magic(pmagic)) {

				flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

				if (blank_entry(pf) && blank_check(pf, required)) {
					return pf;
				}

			}
		}  while (++pmagic != pe);
	}

	return NULL;
}

/****************************************************************************
 * Name: get_next_sector_descriptor
 *
 * Description:
 *   Given a pointer to sector_descriptor_t, this helper function
 *   returns a pointer to the next sector_descriptor_t
 *
 * Input Parameters:
 *   current      - A pointer to the current sector_descriptor_t
 *
 * Returned value:
 *  On Success A pointer to the next sector_descriptor_t,
 *  otherwise NULL
 *
 *
 ****************************************************************************/

static sector_descriptor_t *get_next_sector_descriptor(sector_descriptor_t *
		current)
{
	for (int s = 0; sector_map[s].address; s++) {
		if (current == &sector_map[s]) {
			if (sector_map[s + 1].address) {
				s++;

			} else {
				s = 0;
			}

			return &sector_map[s];
		}
	}

	return NULL;
}

/****************************************************************************
 * Name: get_next_sector
 *
 * Description:
 *   Given a pointer to a flash entry header returns the sector descriptor
 *   for the file is located in
 *
 * Input Parameters:
 *   current      - A pointer to the current flash entry header
 *
 * Returned value:
 *  On Success A pointer to the next sector_descriptor_t,
 *  otherwise NULL
 *
 *
 ****************************************************************************/

static sector_descriptor_t *get_sector_info(flash_entry_header_t *current)
{
	for (int s = 0; sector_map[s].address != 0; s++) {
		uint8_t *pb = (uint8_t *) sector_map[s].address;
		uint8_t *pe = pb + sector_map[s].size - 1;
		uint8_t *pc = (uint8_t *) current;

		if (pc >= pb && pc <= pe) {
			return &sector_map[s];
		}
	}

	return 0;
}

/****************************************************************************
 * Name: erase_sector
 *
 * Description:
 *   Given a pointer to sector_descriptor_t, this function
 *   erases the sector and updates the last_erased using
 *   the pointer to the flash_entry_header_t as a sanity check
 *
 * Input Parameters:
 *   sm      - A pointer to the current sector_descriptor_t
 *   pf      - A pointer to the current flash entry header
 *
 * Returned value:
 *  O On Success or a negative errno
 *
 *
 ****************************************************************************/

// static int erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
// {
// 	int rv = 0;
// 	int16_t page = GetSector((uint32_t)pf);
//     FLASH_EraseInitTypeDef pEraseInit;
//     uint32_t SectorError;

// 	if (page > 0 && page == sm->page) {
//         HAL_FLASH_Unlock(); 
        
// 		last_erased = sm->page;
        
//         pEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
// 		pEraseInit.PageAddress = sm->address;
// 		pEraseInit.NbPages = 1;
    
//         if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
//         {
//             /* Error occurred while page erase */
//             rv = -1;
//         }

// 		HAL_FLASH_Lock();
// 	}

// 	return rv;
// }
static int erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
{
	int rv = 0;
	int16_t page = GetSector((uint32_t)pf);
    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t SectorError;

	if (page > 0 && page == sm->page) {
        HAL_FLASH_Unlock(); 
        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

		last_erased = sm->page;
        pEraseInit.TypeErase = TYPEERASE_SECTORS;
        pEraseInit.Sector = sm->page;
        pEraseInit.NbSectors = 1;
        pEraseInit.VoltageRange = VOLTAGE_RANGE_3;
    
        if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
        {
            /* Error occurred while page erase */
            rv = -1;
        }

		HAL_FLASH_Lock();
	}

	return rv;
}

/****************************************************************************
 * Name: erase_entry
 *
 * Description:
 *   Given a pointer to a flash entry header erases the entry
 *
 * Input Parameters:
 *   pf  - A pointer to the current flash entry header
 *
 *
 * Returned value:
 *  >0 On Success or a negative errno
 *
 *
 ****************************************************************************/

static int erase_entry(flash_entry_header_t *pf)
{
	h_flag_t data = ErasedEntry;
	size_t size = sizeof(h_flag_t);
	int rv = up_progmem_write((size_t) &pf->flag, &data, size);
    
	return rv;
}

/****************************************************************************
 * Name: check_free_space_in_sector
 *
 * Description:
 *   Given a pointer to a flash entry header and a new size
 *
 * Input Parameters:
*   pf       - A pointer to the current flash entry header
 *   new_size - The total number of bytes to be written
  *
 * Returned value:
 *  0 if there is enough space left to write new size
 *  If not it returns the flash_file_sector_t * that needs to be erased.
 *
 ****************************************************************************/

static sector_descriptor_t *check_free_space_in_sector(flash_entry_header_t
		*pf, size_t new_size)
{
	sector_descriptor_t *sm = get_sector_info(pf);
	uint8_t *psector_first = (uint8_t *) sm->address;
	uint8_t *psector_last = psector_first + sm->size - 1;
	uint8_t *pnext_end = (uint8_t *)(valid_magic((h_magic_t *)pf) ? next_entry(pf) : pf) + new_size;

	if (pnext_end >= psector_first && pnext_end <= psector_last) {
		sm = 0;
	}

	return sm;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
/****************************************************************************
 * Name: parameter_flashfs_read
 *
 * Description:
 *   This function returns a pointer to the locations of the data associated
 *   with the file token. On successful return *buffer will be set to Flash
 *   location and *buf_size the length of the user data.
 *
 * Input Parameters:
 *   token       - File Token File to read
 *   buffer      - A pointer to a pointer that will receive the address
 *                 in flash of the data of this "files" data
 *   buf_size    - A pointer to receive the number of bytes in the "file"
 *
 * Returned value:
 *   On success number of bytes read or a negative errno value,
 *
 *
 ****************************************************************************/

int parameter_flashfs_read(flash_file_token_t token, uint8_t **buffer, size_t
			   *buf_size)
{
	int rv = -1;

	if (sector_map) {

		rv = -1;
		flash_entry_header_t *pf = find_entry(token);

		if (pf) {
			(*buffer) = entry_data(pf);
			rv = entry_data_length(pf);
			*buf_size = rv;
		}
	}

	return rv;
}

/****************************************************************************
 * Name: parameter_flashfs_write
 *
 * Description:
 *   This function writes user data from the buffer allocated with a previous call
 *   to parameter_flashfs_alloc. flash starting at the given address
 *
 * Input Parameters:
 *   token      - File Token File to read
 *   buffer      - A pointer to a buffer with buf_size bytes to be written
 *                 to the flash. This buffer must be allocated
 *                 with a previous call to parameter_flashfs_alloc
 *   buf_size    - Number of bytes to write
 *
 * Returned value:
 *   On success the number of bytes written On Error a negative value of errno
 *
 ****************************************************************************/

int
parameter_flashfs_write(flash_file_token_t token, uint8_t *buffer, size_t buf_size)
{
	int rv = -1;

	if (sector_map) {

		rv = 0;

		/* Calculate the total space needed */

		size_t total_size = buf_size + sizeof(flash_entry_header_t);
		size_t alignment = sizeof(h_magic_t) - 1;
		size_t  size_adjust = ((total_size + alignment) & ~alignment) - total_size;
		total_size += size_adjust;

		/* Is this and existing entry */

		flash_entry_header_t *pf = find_entry(token);

		if (!pf) {

			/* No Entry exists for this token so find a place for it */

			pf = find_free(total_size);

			/* No Space */

			if (pf == 0) {
				return -1;
			}

		} else {

			/* Do we have space after the entry in the sector for the update */

			sector_descriptor_t *current_sector = check_free_space_in_sector(pf,
							      total_size);


			if (current_sector == 0) {

				/* Mark the last entry erased */

				/* todo:consider a 2 stage erase or write before erase and do a fs check
				 * at start up
				 */

				rv = erase_entry(pf);

				if (rv < 0) {
					return rv;
				}

				/* We had space and marked the last entry erased so use the  Next Free */

				pf = next_entry(pf);

			} else {

				/*
				 * We did not have space in the current sector so select the next sector
				 */

				current_sector = get_next_sector_descriptor(current_sector);

				/* Will the data fit */

				if (current_sector->size < total_size) {
					return -1;
				}

				/* Mark the last entry erased */

				/* todo:consider a 2 stage erase or write before erase and do a fs check
				 * at start up
				 */

				rv = erase_entry(pf);

				if (rv < 0) {
					return rv;
				}

				pf = (flash_entry_header_t *) current_sector->address;
			}

			if (!blank_check(pf, total_size)) {
				rv = erase_sector(current_sector, pf);
			}
		}

		flash_entry_header_t *pn = (flash_entry_header_t *)(buffer - sizeof(flash_entry_header_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = ValidEntry + size_adjust;
		pn->size = total_size;

		for (size_t a = 0; a < size_adjust; a++) {
			buffer[buf_size + a] = (uint8_t)BlankSig;
		}

		pn->crc = crc32(entry_crc_start(pn), entry_crc_length(pn));
		rv = up_progmem_write((size_t) pf, pn, pn->size);
		int system_bytes = (sizeof(flash_entry_header_t) + size_adjust);

		if (rv >= system_bytes) {
			rv -= system_bytes;
		}
	}

	return rv;
}

/****************************************************************************
 * Name: parameter_flashfs_alloc
 *
 * Description:
 *   This function is called to get a buffer to use in a subsequent call
 *   to parameter_flashfs_write. The address returned is advanced into the
 *   buffer to reserve space for the flash entry header.
 *   The caller is responsible to call parameter_flashfs_free after usage.
 *
 * Input Parameters:
 *   token      - File Token File to read (not used)
 *   buffer     - A pointer to return a pointer to Memory of buf_size length
 *   			  suitable for calling parameter_flashfs_write
 *   buf_size   - In: the size needed for the write operation.
 *   			  Out: The maximum number of bytes that can be written to
 *                the buffer
 *
 * Returned value:
 *   On success the number of bytes written On Error a negative value of errno
 *
 ****************************************************************************/

int parameter_flashfs_alloc(flash_file_token_t token, uint8_t **buffer, size_t *buf_size)
{
	int rv = -1;

	if (sector_map) {

		rv = -1;

		if (!working_buffer_static) {

			working_buffer_size = *buf_size + sizeof(flash_entry_header_t);
			working_buffer = pvPortMalloc(working_buffer_size);
		}

		/* Allocation failed or not provided */

		if (working_buffer == NULL) {

			working_buffer_size = 0;

		} else {

			/* We have a buffer reserve space and init it */
			*buffer = &working_buffer[sizeof(flash_entry_header_t)];
			*buf_size = working_buffer_size - sizeof(flash_entry_header_t);
			memset(working_buffer, 0xff, working_buffer_size);
			rv = 0;

		}
	}

	return rv;
}

/****************************************************************************
 * Name: parameter_flashfs_erase
 *
 * Description:
 *   This function erases the sectors that were passed to parameter_flashfs_init
 *
 * Input Parameters:
 *
 * Returned value:
 *   On success the number of bytes erased
 *   On Error a negative value of errno
 *
 ****************************************************************************/
int parameter_flashfs_erase(void)
{
	int rv = -1;

	if (sector_map) {
		rv = 0;
        
		for (int s = 0; sector_map[s].address; s++) {
			int sz = erase_sector(&sector_map[s], (flash_entry_header_t *)sector_map[s].address);

			if (sz != 0) {
				return sz;
			}

			rv += sector_map[s].size;
		}
	}

	return rv;
}

/****************************************************************************
 * Name: parameter_flashfs_init
 *
 * Description:
 *   This helper function advances the flash entry header pointer to the
 *   locations of the next entry.
 *
 * Input Parameters:
 *   fconfig      - A pointer to an null entry terminated array of
 *                  flash_file_sector_t
 *    buffer      - A pointer to a memory to make available to callers
 *                  for write operations. When allocated to the caller
 *                  space is reserved in the front for the
 *                  flash_entry_header_t.
 *                  If this is passes as NULL. The buffer will be
 *                  allocated from the heap on calls to
 *                  parameter_flashfs_alloc and fread on calls
 *                  to parameter_flashfs_free
 *
 *   size         - The size of the buffer in bytes. Should be be 0 if buffer
 *                  is NULL
 *
 * Returned value:
 *                - A pointer to the next file header location
 *
 *
 ****************************************************************************/
int parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *buffer, uint16_t size)
{
	int rv = 1;
	sector_map = fconfig;
	working_buffer_static = buffer != NULL;

	if (!working_buffer_static) {
		size = 0;
	}

	working_buffer = buffer;
	working_buffer_size = size;
	last_erased = -1;

	/* Sanity check */

	flash_entry_header_t *pf = find_entry(parameters_token);

	/*  No paramaters */

	if (pf == NULL) {
		size_t total_size = size + sizeof(flash_entry_header_t);
		size_t alignment = sizeof(h_magic_t) - 1;
		size_t  size_adjust = ((total_size + alignment) & ~alignment) - total_size;
		total_size += size_adjust;

		/* Do we have free space ?*/

		if (find_free(total_size) == NULL) {

			/* No paramates and no free space => neeed erase */

			rv  = parameter_flashfs_erase();
		}
	}

	return rv;
}

/*
* Bootloader(sector 0):
* start: 0x08000000, len: 16K, end: 0x08003E80
*
* Firmware(sector 1 and 10):
* start: 0x08004000, len: 32K, end: 0x080DFFFF
*
* FlashFS(sector 11):
* start: 0x080E0000, len: 128K, end: 0x080FFFFF
*
*/
/****************************************************************************
 * Public Data
 ****************************************************************************/
// static sector_descriptor_t params_sector_map[] = {
//     {250, 2 * 1024, 0x0807D000},  // 2K Bytes 0x0807D000 - 0x0807D7FF
// 	{251, 2 * 1024, 0x0807D800},  // 2K Bytes 0x0807D800 - 0x0807DFFF
// 	{252, 2 * 1024, 0x0807E000},  // 2K Bytes 0x0807E000 - 0x0807E7FF
// 	{253, 2 * 1024, 0x0807E800},  // 2K Bytes 0x0807E800 - 0x0807EFFF
// 	{254, 2 * 1024, 0x0807F000},  // 2K Bytes 0x0807F000 - 0x0807F7FF
// 	{255, 2 * 1024, 0x0807F800},  // 2K Bytes 0x0807F800 - 0x0807FFFF
//     {0, 0, 0},
// };

static sector_descriptor_t params_sector_map[] = {
    {11, 128 * 1024, 0x080E0000},
    {0, 0, 0},
};

int flashfs_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 0;
	}
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {

            if(parameter_flashfs_init(params_sector_map, NULL, 0))
                Info_Debug("F-RAM file system init successed\n");
            else {
                Info_Debug("F-RAM file system init failed\n");
                return 0;
            }
        }
        
        if (!strcmp(argv[i], "erase")) {
			if(parameter_flashfs_erase()) {
				Info_Debug("F-RAM file system erase successed\n");
			} else {
                Info_Debug("F-RAM file system erase failed\n");
				return 0;
			}
        }
    }
    
    return 1;
}

