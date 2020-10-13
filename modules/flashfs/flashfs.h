#ifndef _FLASHFS_H
#define _FLASHFS_H

#include <stdint.h>
#include <stdbool.h>

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

#ifdef __cplusplus
extern  "C" {
#endif

/*
 *  Define the interface data a flash_file_token_t
 *  is like a file name
 *
 */
typedef uint32_t flash_file_tokens_t;

typedef struct flash_file_token_t {
	union {
		flash_file_tokens_t t;
		uint8_t n[sizeof(flash_file_tokens_t)];
	};
} flash_file_token_t;

/*
 * Define the parameter "file name" Currently there is only
 * and it is hard coded. If more are added the
 * parameter_flashfs_write would need to support a backing buffer
 * for when a sector is erased.
 */
extern const flash_file_token_t parameters_token;

/* Define the elements of the array passed to the
 * parameter_flashfs_init function
 *
 * For example
 * static sector_descriptor_t  sector_map[] = {
 *      {1, 16 * 1024, 0x08004000},
 *      {2, 16 * 1024, 0x08008000},
 *       {0, 0, 0},
 *
 */
typedef struct sector_descriptor_t {
	uint8_t       page;
	uint32_t      size;
	uint32_t      address;
} sector_descriptor_t;


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

int parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *buffer, uint16_t size);

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

int parameter_flashfs_read(flash_file_token_t ft, uint8_t **buffer, size_t *buf_size);

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

int parameter_flashfs_write(flash_file_token_t ft, uint8_t *buffer, size_t buf_size);

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

int parameter_flashfs_erase(void);

/****************************************************************************
 * Name: parameter_flashfs_alloc
 *
 * Description:
 *   This function is called to get a buffer to use in a subsequent call
 *   to parameter_flashfs_write. The address returned is advanced into the
 *   buffer to reserve space for the flash entry header.
 *
 * Input Parameters:
 *   token      - File Token File to read (not used)
 *   buffer     - Memory of buf_size length suitable for calling
 *                parameter_flashfs_write
 *   buf_size   - The maximum number of bytes that can be written to
 *                the buffer
 *
 * Returned value:
 *   On success the number of bytes written On Error a negative value of errno
 *
 ****************************************************************************/

int parameter_flashfs_alloc(flash_file_token_t ft, uint8_t **buffer, size_t *buf_size);


/****************************************************************************
 * Name: parameter_flashfs_free
 *
 * Description:
 *   Frees  dynamically allocated memory
 *
 *
 ****************************************************************************/

void parameter_flashfs_free(void);

#ifdef __cplusplus
}
#endif
#endif