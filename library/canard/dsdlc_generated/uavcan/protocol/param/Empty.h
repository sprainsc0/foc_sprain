/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\param\Empty.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_PARAM_EMPTY
#define __UAVCAN_PROTOCOL_PARAM_EMPTY

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# Ex nihilo nihil fit.
#
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.param.Empty
******************************************************************************/

#define UAVCAN_PROTOCOL_PARAM_EMPTY_NAME                   "uavcan.protocol.param.Empty"
#define UAVCAN_PROTOCOL_PARAM_EMPTY_SIGNATURE              (0x6C4D0E8EF37361DFULL)

#define UAVCAN_PROTOCOL_PARAM_EMPTY_MAX_SIZE               ((0 + 7)/8)

typedef struct
{
    uint8_t empty;
} uavcan_protocol_param_Empty;

static inline
uint32_t uavcan_protocol_param_Empty_encode(uavcan_protocol_param_Empty* source, void* msg_buf);

static inline
int32_t uavcan_protocol_param_Empty_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_param_Empty* dest, uint8_t** dyn_arr_buf);

static inline
uint32_t uavcan_protocol_param_Empty_encode_internal(uavcan_protocol_param_Empty* source, void* msg_buf, uint32_t offset, uint8_t root_item);

static inline
int32_t uavcan_protocol_param_Empty_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_param_Empty* dest, uint8_t** dyn_arr_buf, int32_t offset);



/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\param\Empty.uavcan
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

uint32_t uavcan_protocol_param_Empty_encode_internal(uavcan_protocol_param_Empty* CANARD_MAYBE_UNUSED(source),
  void* CANARD_MAYBE_UNUSED(msg_buf),
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    return offset;
}

uint32_t uavcan_protocol_param_Empty_encode(uavcan_protocol_param_Empty* CANARD_MAYBE_UNUSED(source), void* CANARD_MAYBE_UNUSED(msg_buf))
{
    return 0;
}

int32_t uavcan_protocol_param_Empty_decode_internal(const CanardRxTransfer* CANARD_MAYBE_UNUSED(transfer),
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_param_Empty* CANARD_MAYBE_UNUSED(dest),
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset)
{
    return offset;
}

int32_t uavcan_protocol_param_Empty_decode(const CanardRxTransfer* CANARD_MAYBE_UNUSED(transfer),
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_param_Empty* CANARD_MAYBE_UNUSED(dest),
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf))
{
    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_PARAM_EMPTY
