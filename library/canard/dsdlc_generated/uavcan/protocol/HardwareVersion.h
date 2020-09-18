/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\HardwareVersion.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_HARDWAREVERSION
#define __UAVCAN_PROTOCOL_HARDWAREVERSION

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# Nested type.
# Generic hardware version information.
# These values should remain unchanged for the device's lifetime.
#

#
# Hardware version code.
#
uint8 major
uint8 minor

#
# Unique ID is a 128 bit long sequence that is globally unique for each node.
# All zeros is not a valid UID.
# If filled with zeros, assume that the value is undefined.
#
uint8[16] unique_id

#
# Certificate of authenticity (COA) of the hardware, 255 bytes max.
#
uint8[<=255] certificate_of_authenticity
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.HardwareVersion
saturated uint8 major
saturated uint8 minor
saturated uint8[16] unique_id
saturated uint8[<=255] certificate_of_authenticity
******************************************************************************/

#define UAVCAN_PROTOCOL_HARDWAREVERSION_NAME               "uavcan.protocol.HardwareVersion"
#define UAVCAN_PROTOCOL_HARDWAREVERSION_SIGNATURE          (0xAD5C4C933F4A0C4ULL)

#define UAVCAN_PROTOCOL_HARDWAREVERSION_MAX_SIZE           ((2192 + 7)/8)

// Constants

#define UAVCAN_PROTOCOL_HARDWAREVERSION_UNIQUE_ID_LENGTH                                 16
#define UAVCAN_PROTOCOL_HARDWAREVERSION_CERTIFICATE_OF_AUTHENTICITY_MAX_LENGTH           255

typedef struct
{
    // FieldTypes
    uint8_t    major;                         // bit len 8
    uint8_t    minor;                         // bit len 8
    uint8_t    unique_id[16];                 // Static Array 8bit[16] max items
    struct
    {
        uint8_t    len;                       // Dynamic array length
        uint8_t*   data;                      // Dynamic Array 8bit[255] max items
    } certificate_of_authenticity;

} uavcan_protocol_HardwareVersion;

static inline
uint32_t uavcan_protocol_HardwareVersion_encode(uavcan_protocol_HardwareVersion* source, void* msg_buf);

static inline
int32_t uavcan_protocol_HardwareVersion_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_HardwareVersion* dest, uint8_t** dyn_arr_buf);

static inline
uint32_t uavcan_protocol_HardwareVersion_encode_internal(uavcan_protocol_HardwareVersion* source, void* msg_buf, uint32_t offset, uint8_t root_item);

static inline
int32_t uavcan_protocol_HardwareVersion_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_HardwareVersion* dest, uint8_t** dyn_arr_buf, int32_t offset);

/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\HardwareVersion.uavcan
 */

#ifndef CANARD_INTERNAL_SATURATE
#define CANARD_INTERNAL_SATURATE(x, max) ( ((x) > max) ? max : ( (-(x) > max) ? (-max) : (x) ) );
#endif

#ifndef CANARD_INTERNAL_SATURATE_UNSIGNED
#define CANARD_INTERNAL_SATURATE_UNSIGNED(x, max) ( ((x) > max) ? max : (x) );
#endif

#if defined(__GNUC__)
# define CANARD_MAYBE_UNUSED(x) x __attribute__((unused))
#else
# define CANARD_MAYBE_UNUSED(x) x
#endif

/**
  * @brief uavcan_protocol_HardwareVersion_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_protocol_HardwareVersion_encode_internal(uavcan_protocol_HardwareVersion* source,
  void* msg_buf,
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    uint32_t c = 0;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->major); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->minor); // 255
    offset += 8;

    // Static array (unique_id)
    for (c = 0; c < 16; c++)
    {
        canardEncodeScalar(msg_buf, offset, 8, (void*)(source->unique_id + c)); // 255
        offset += 8;
    }

    // Dynamic Array (certificate_of_authenticity)
    if (! root_item)
    {
        // - Add array length
        canardEncodeScalar(msg_buf, offset, 8, (void*)&source->certificate_of_authenticity.len);
        offset += 8;
    }

    // - Add array items
    for (c = 0; c < source->certificate_of_authenticity.len; c++)
    {
        canardEncodeScalar(msg_buf,
                           offset,
                           8,
                           (void*)(source->certificate_of_authenticity.data + c));// 255
        offset += 8;
    }

    return offset;
}

/**
  * @brief uavcan_protocol_HardwareVersion_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_protocol_HardwareVersion_encode(uavcan_protocol_HardwareVersion* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_protocol_HardwareVersion_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_protocol_HardwareVersion_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_HardwareVersion dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_HardwareVersion_decode_internal(
  const CanardRxTransfer* transfer,
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_HardwareVersion* dest,
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset)
{
    int32_t ret = 0;
    uint32_t c = 0;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->major);
    if (ret != 8)
    {
        goto uavcan_protocol_HardwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->minor);
    if (ret != 8)
    {
        goto uavcan_protocol_HardwareVersion_error_exit;
    }
    offset += 8;

    // Static array (unique_id)
    for (c = 0; c < 16; c++)
    {
        ret = canardDecodeScalar(transfer, offset, 8, false, (void*)(dest->unique_id + c));
        if (ret != 8)
        {
            goto uavcan_protocol_HardwareVersion_error_exit;
        }
        offset += 8;
    }

    // Dynamic Array (certificate_of_authenticity)
    //  - Last item in struct & Root item & (Array Size > 8 bit), tail array optimization
    if (payload_len)
    {
        //  - Calculate Array length from MSG length
        dest->certificate_of_authenticity.len = ((payload_len * 8) - offset ) / 8; // 8 bit array item size
    }
    else
    {
        // - Array length 8 bits
        ret = canardDecodeScalar(transfer,
                                 offset,
                                 8,
                                 false,
                                 (void*)&dest->certificate_of_authenticity.len); // 255
        if (ret != 8)
        {
            goto uavcan_protocol_HardwareVersion_error_exit;
        }
        offset += 8;
    }

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->certificate_of_authenticity.data = (uint8_t*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->certificate_of_authenticity.len; c++)
    {
        if (dyn_arr_buf)
        {
            ret = canardDecodeScalar(transfer,
                                     offset,
                                     8,
                                     false,
                                     (void*)*dyn_arr_buf); // 255
            if (ret != 8)
            {
                goto uavcan_protocol_HardwareVersion_error_exit;
            }
            *dyn_arr_buf = (uint8_t*)(((uint8_t*)*dyn_arr_buf) + 1);
        }
        offset += 8;
    }
    return offset;

uavcan_protocol_HardwareVersion_error_exit:
    if (ret < 0)
    {
        return ret;
    }
    else
    {
        return -CANARD_ERROR_INTERNAL;
    }
}

/**
  * @brief uavcan_protocol_HardwareVersion_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_HardwareVersion dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_HardwareVersion_decode(const CanardRxTransfer* transfer,
  uint16_t payload_len,
  uavcan_protocol_HardwareVersion* dest,
  uint8_t** dyn_arr_buf)
{
    const int32_t offset = 0;
    int32_t ret = 0;

    // Clear the destination struct
    for (uint32_t c = 0; c < sizeof(uavcan_protocol_HardwareVersion); c++)
    {
        ((uint8_t*)dest)[c] = 0x00;
    }

    ret = uavcan_protocol_HardwareVersion_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset);

    return ret;
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_HARDWAREVERSION