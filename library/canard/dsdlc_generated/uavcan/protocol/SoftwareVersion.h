/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\SoftwareVersion.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_SOFTWAREVERSION
#define __UAVCAN_PROTOCOL_SOFTWAREVERSION

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# Nested type.
# Generic software version information.
#

#
# Primary version numbers.
# If both fields are set to zero, the version is considered unknown.
#
uint8 major
uint8 minor

#
# This mask indicates which optional fields (see below) are set.
#
uint8 OPTIONAL_FIELD_FLAG_VCS_COMMIT = 1
uint8 OPTIONAL_FIELD_FLAG_IMAGE_CRC  = 2
uint8 optional_field_flags

#
# VCS commit hash or revision number, e.g. git short commit hash. Optional.
#
uint32 vcs_commit

#
# The value of an arbitrary hash function applied to the firmware image.
# This field is used to detect whether the firmware running on the node is EXACTLY THE SAME
# as a certain specific revision. This field provides the absolute identity guarantee, unlike
# the version fields above, which can be the same for different builds of the firmware.
#
# The exact hash function and the methods of its application are implementation defined.
# However, implementations are recommended to adhere to the following guidelines,
# fully or partially:
#
#   - The hash function should be CRC-64-WE, the same that is used for computing DSDL signatures.
#
#   - The hash function should be applied to the entire application image padded to 8 bytes.
#
#   - If the computed image CRC is stored within the firmware image itself, the value of
#     the hash function becomes ill-defined, because it becomes recursively dependent on itself.
#     In order to circumvent this issue, while computing or checking the CRC, its value stored
#     within the image should be zeroed out.
#
uint64 image_crc
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.SoftwareVersion
saturated uint8 major
saturated uint8 minor
saturated uint8 optional_field_flags
saturated uint32 vcs_commit
saturated uint64 image_crc
******************************************************************************/

#define UAVCAN_PROTOCOL_SOFTWAREVERSION_NAME               "uavcan.protocol.SoftwareVersion"
#define UAVCAN_PROTOCOL_SOFTWAREVERSION_SIGNATURE          (0xDD46FD376527FEA1ULL)

#define UAVCAN_PROTOCOL_SOFTWAREVERSION_MAX_SIZE           ((120 + 7)/8)

// Constants
#define UAVCAN_PROTOCOL_SOFTWAREVERSION_OPTIONAL_FIELD_FLAG_VCS_COMMIT          1 // 1
#define UAVCAN_PROTOCOL_SOFTWAREVERSION_OPTIONAL_FIELD_FLAG_IMAGE_CRC          2 // 2

typedef struct
{
    // FieldTypes
    uint8_t    major;                         // bit len 8
    uint8_t    minor;                         // bit len 8
    uint8_t    optional_field_flags;          // bit len 8
    uint32_t   vcs_commit;                    // bit len 32
    uint64_t   image_crc;                     // bit len 64

} uavcan_protocol_SoftwareVersion;

static inline
uint32_t uavcan_protocol_SoftwareVersion_encode(uavcan_protocol_SoftwareVersion* source, void* msg_buf);

static inline
int32_t uavcan_protocol_SoftwareVersion_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_SoftwareVersion* dest, uint8_t** dyn_arr_buf);

static inline
uint32_t uavcan_protocol_SoftwareVersion_encode_internal(uavcan_protocol_SoftwareVersion* source, void* msg_buf, uint32_t offset, uint8_t root_item);

static inline
int32_t uavcan_protocol_SoftwareVersion_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_SoftwareVersion* dest, uint8_t** dyn_arr_buf, int32_t offset);

/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\SoftwareVersion.uavcan
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
  * @brief uavcan_protocol_SoftwareVersion_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_protocol_SoftwareVersion_encode_internal(uavcan_protocol_SoftwareVersion* source,
  void* msg_buf,
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->major); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->minor); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->optional_field_flags); // 255
    offset += 8;

    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->vcs_commit); // 4294967295
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 64, (void*)&source->image_crc); // 18446744073709551615
    offset += 64;

    return offset;
}

/**
  * @brief uavcan_protocol_SoftwareVersion_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_protocol_SoftwareVersion_encode(uavcan_protocol_SoftwareVersion* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_protocol_SoftwareVersion_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_protocol_SoftwareVersion_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_SoftwareVersion dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_SoftwareVersion_decode_internal(
  const CanardRxTransfer* transfer,
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_SoftwareVersion* dest,
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset)
{
    int32_t ret = 0;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->major);
    if (ret != 8)
    {
        goto uavcan_protocol_SoftwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->minor);
    if (ret != 8)
    {
        goto uavcan_protocol_SoftwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->optional_field_flags);
    if (ret != 8)
    {
        goto uavcan_protocol_SoftwareVersion_error_exit;
    }
    offset += 8;

    ret = canardDecodeScalar(transfer, offset, 32, false, (void*)&dest->vcs_commit);
    if (ret != 32)
    {
        goto uavcan_protocol_SoftwareVersion_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 64, false, (void*)&dest->image_crc);
    if (ret != 64)
    {
        goto uavcan_protocol_SoftwareVersion_error_exit;
    }
    offset += 64;
    return offset;

uavcan_protocol_SoftwareVersion_error_exit:
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
  * @brief uavcan_protocol_SoftwareVersion_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_SoftwareVersion dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_SoftwareVersion_decode(const CanardRxTransfer* transfer,
  uint16_t payload_len,
  uavcan_protocol_SoftwareVersion* dest,
  uint8_t** dyn_arr_buf)
{
    const int32_t offset = 0;
    int32_t ret = 0;

    // Clear the destination struct
    for (uint32_t c = 0; c < sizeof(uavcan_protocol_SoftwareVersion); c++)
    {
        ((uint8_t*)dest)[c] = 0x00;
    }

    ret = uavcan_protocol_SoftwareVersion_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset);

    return ret;
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_SOFTWAREVERSION