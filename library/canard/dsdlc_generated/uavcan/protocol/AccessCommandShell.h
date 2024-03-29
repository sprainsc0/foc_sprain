/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\6.AccessCommandShell.uavcan
 */

#ifndef __UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL
#define __UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL

#include <stdint.h>
#include "canard.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************* Source text **********************************
#
# THIS DEFINITION IS SUBJECT TO CHANGE.
#
# This service allows to execute arbitrary commands on the remote node's internal system shell.
#
# Essentially, this service mimics a typical terminal emulator, with one text input (stdin) and two text
# outputs (stdout and stderr). When there's no process running, the input is directed into the terminal
# handler itself, which interprets it. If there's a process running, the input will be directed into
# stdin of the running process. It is possible to forcefully return the terminal into a known state by
# means of setting the reset flag (see below), in which case the terminal will kill all of the child
# processes, if any, and return into the initial idle state.
#
# The server is assumed to allocate one independent terminal instance per client, so that different clients
# can execute commands without interfering with each other.
#

#
# Input and output should use this newline character.
#
uint8 NEWLINE = '\n'

#
# The server is required to keep the result of the last executed command for at least this time.
# When this time expires, the server may remove the results in order to reclaim the memory, but it
# is not guaranteed. Hence, the clients must retrieve the results in this amount of time.
#
uint8 MIN_OUTPUT_LIFETIME_SEC = 10

#
# These flags control the shell and command execution.
#
uint8 FLAG_RESET_SHELL          = 1     # Restarts the shell instance anew; may or may not imply CLEAR_OUTPUT_BUFFERS
uint8 FLAG_CLEAR_OUTPUT_BUFFERS = 2     # Makes stdout and stderr buffers empty
uint8 FLAG_READ_STDOUT          = 64    # Output will contain stdout
uint8 FLAG_READ_STDERR          = 128   # Output will be extended with stderr
uint8 flags

#
# If the shell is idle, it will interpret this string.
# If there's a process running, this string will be piped into its stdin.
#
# If RESET_SHELL is set, new input will be interpreted by the shell immediately.
#
uint8[<=128] input

---

#
# Exit status of the last executed process, or error code of the shell itself.
# Default value is zero.
#
int32 last_exit_status

#
# These flags indicate the status of the shell.
#
uint8 FLAG_RUNNING              = 1     # The shell is currently running a process; stdin/out/err are piped to it
uint8 FLAG_SHELL_ERROR          = 2     # Exit status contains error code, output contains text (e.g. no such command)
uint8 FLAG_HAS_PENDING_STDOUT   = 64    # There is more stdout to read
uint8 FLAG_HAS_PENDING_STDERR   = 128   # There is more stderr to read
uint8 flags

#
# In case of a shell error, this string may contain ASCII string explaining the nature of the error.
# Otherwise, if stdout read is requested, this string will contain stdout data. If stderr read is requested,
# this string will contain stderr data. If both stdout and stderr read is requested, this string will start
# with stdout and end with stderr, with no separator in between.
#
uint8[<=256] output
******************************************************************************/

/********************* DSDL signature source definition ***********************
uavcan.protocol.AccessCommandShell
saturated uint8 flags
saturated uint8[<=128] input
---
saturated int32 last_exit_status
saturated uint8 flags
saturated uint8[<=256] output
******************************************************************************/

#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_ID              6
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_NAME            "uavcan.protocol.AccessCommandShell"
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_SIGNATURE       (0x59276B5921C9246EULL)

#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_MAX_SIZE ((1040 + 7)/8)

// Constants
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_NEWLINE                 '\n' // '\n'
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_MIN_OUTPUT_LIFETIME_SEC         10 // 10
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_FLAG_RESET_SHELL           1 // 1
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_FLAG_CLEAR_OUTPUT_BUFFERS          2 // 2
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_FLAG_READ_STDOUT          64 // 64
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_FLAG_READ_STDERR         128 // 128

#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_REQUEST_INPUT_MAX_LENGTH                      128

typedef struct
{
    // FieldTypes
    uint8_t    flags;                         // bit len 8
    struct
    {
        uint8_t    len;                       // Dynamic array length
        uint8_t*   data;                      // Dynamic Array 8bit[128] max items
    } input;

} uavcan_protocol_AccessCommandShellRequest;

static inline
uint32_t uavcan_protocol_AccessCommandShellRequest_encode(uavcan_protocol_AccessCommandShellRequest* source, void* msg_buf);

static inline
int32_t uavcan_protocol_AccessCommandShellRequest_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_AccessCommandShellRequest* dest, uint8_t** dyn_arr_buf);

static inline
uint32_t uavcan_protocol_AccessCommandShellRequest_encode_internal(uavcan_protocol_AccessCommandShellRequest* source, void* msg_buf, uint32_t offset, uint8_t root_item);

static inline
int32_t uavcan_protocol_AccessCommandShellRequest_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_AccessCommandShellRequest* dest, uint8_t** dyn_arr_buf, int32_t offset);

#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_RESPONSE_MAX_SIZE ((2097 + 7)/8)

// Constants
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_RESPONSE_FLAG_RUNNING              1 // 1
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_RESPONSE_FLAG_SHELL_ERROR          2 // 2
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_RESPONSE_FLAG_HAS_PENDING_STDOUT         64 // 64
#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_RESPONSE_FLAG_HAS_PENDING_STDERR        128 // 128

#define UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL_RESPONSE_OUTPUT_MAX_LENGTH                    256

typedef struct
{
    // FieldTypes
    int32_t    last_exit_status;              // bit len 32
    uint8_t    flags;                         // bit len 8
    struct
    {
        uint16_t    len;                       // Dynamic array length
        uint8_t*   data;                      // Dynamic Array 8bit[256] max items
    } output;

} uavcan_protocol_AccessCommandShellResponse;

static inline
uint32_t uavcan_protocol_AccessCommandShellResponse_encode(uavcan_protocol_AccessCommandShellResponse* source, void* msg_buf);

static inline
int32_t uavcan_protocol_AccessCommandShellResponse_decode(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_AccessCommandShellResponse* dest, uint8_t** dyn_arr_buf);

static inline
uint32_t uavcan_protocol_AccessCommandShellResponse_encode_internal(uavcan_protocol_AccessCommandShellResponse* source, void* msg_buf, uint32_t offset, uint8_t root_item);

static inline
int32_t uavcan_protocol_AccessCommandShellResponse_decode_internal(const CanardRxTransfer* transfer, uint16_t payload_len, uavcan_protocol_AccessCommandShellResponse* dest, uint8_t** dyn_arr_buf, int32_t offset);

/*
 * UAVCAN data structure definition for libcanard.
 *
 * Autogenerated, do not edit.
 *
 * Source file: f:\SPRainCore\bgc_sprain\bgc\module\canard\dsdl\uavcan\protocol\6.AccessCommandShell.uavcan
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
  * @brief uavcan_protocol_AccessCommandShellRequest_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_protocol_AccessCommandShellRequest_encode_internal(uavcan_protocol_AccessCommandShellRequest* source,
  void* msg_buf,
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    uint32_t c = 0;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->flags); // 255
    offset += 8;

    // Dynamic Array (input)
    if (! root_item)
    {
        // - Add array length
        canardEncodeScalar(msg_buf, offset, 8, (void*)&source->input.len);
        offset += 8;
    }

    // - Add array items
    for (c = 0; c < source->input.len; c++)
    {
        canardEncodeScalar(msg_buf,
                           offset,
                           8,
                           (void*)(source->input.data + c));// 255
        offset += 8;
    }

    return offset;
}

/**
  * @brief uavcan_protocol_AccessCommandShellRequest_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_protocol_AccessCommandShellRequest_encode(uavcan_protocol_AccessCommandShellRequest* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_protocol_AccessCommandShellRequest_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_protocol_AccessCommandShellRequest_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_AccessCommandShellRequest dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_AccessCommandShellRequest_decode_internal(
  const CanardRxTransfer* transfer,
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_AccessCommandShellRequest* dest,
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset)
{
    int32_t ret = 0;
    uint32_t c = 0;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->flags);
    if (ret != 8)
    {
        goto uavcan_protocol_AccessCommandShellRequest_error_exit;
    }
    offset += 8;

    // Dynamic Array (input)
    //  - Last item in struct & Root item & (Array Size > 8 bit), tail array optimization
    if (payload_len)
    {
        //  - Calculate Array length from MSG length
        dest->input.len = ((payload_len * 8) - offset ) / 8; // 8 bit array item size
    }
    else
    {
        // - Array length 8 bits
        ret = canardDecodeScalar(transfer,
                                 offset,
                                 8,
                                 false,
                                 (void*)&dest->input.len); // 255
        if (ret != 8)
        {
            goto uavcan_protocol_AccessCommandShellRequest_error_exit;
        }
        offset += 8;
    }

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->input.data = (uint8_t*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->input.len; c++)
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
                goto uavcan_protocol_AccessCommandShellRequest_error_exit;
            }
            *dyn_arr_buf = (uint8_t*)(((uint8_t*)*dyn_arr_buf) + 1);
        }
        offset += 8;
    }
    return offset;

uavcan_protocol_AccessCommandShellRequest_error_exit:
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
  * @brief uavcan_protocol_AccessCommandShellRequest_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_AccessCommandShellRequest dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_AccessCommandShellRequest_decode(const CanardRxTransfer* transfer,
  uint16_t payload_len,
  uavcan_protocol_AccessCommandShellRequest* dest,
  uint8_t** dyn_arr_buf)
{
    const int32_t offset = 0;
    int32_t ret = 0;

    // Clear the destination struct
    for (uint32_t c = 0; c < sizeof(uavcan_protocol_AccessCommandShellRequest); c++)
    {
        ((uint8_t*)dest)[c] = 0x00;
    }

    ret = uavcan_protocol_AccessCommandShellRequest_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset);

    return ret;
}

/**
  * @brief uavcan_protocol_AccessCommandShellResponse_encode_internal
  * @param source : pointer to source data struct
  * @param msg_buf: pointer to msg storage
  * @param offset: bit offset to msg storage
  * @param root_item: for detecting if TAO should be used
  * @retval returns offset
  */
uint32_t uavcan_protocol_AccessCommandShellResponse_encode_internal(uavcan_protocol_AccessCommandShellResponse* source,
  void* msg_buf,
  uint32_t offset,
  uint8_t CANARD_MAYBE_UNUSED(root_item))
{
    uint32_t c = 0;

    canardEncodeScalar(msg_buf, offset, 32, (void*)&source->last_exit_status); // 2147483647
    offset += 32;

    canardEncodeScalar(msg_buf, offset, 8, (void*)&source->flags); // 255
    offset += 8;

    // Dynamic Array (output)
    if (! root_item)
    {
        // - Add array length
        canardEncodeScalar(msg_buf, offset, 9, (void*)&source->output.len);
        offset += 9;
    }

    // - Add array items
    for (c = 0; c < source->output.len; c++)
    {
        canardEncodeScalar(msg_buf,
                           offset,
                           8,
                           (void*)(source->output.data + c));// 255
        offset += 8;
    }

    return offset;
}

/**
  * @brief uavcan_protocol_AccessCommandShellResponse_encode
  * @param source : Pointer to source data struct
  * @param msg_buf: Pointer to msg storage
  * @retval returns message length as bytes
  */
uint32_t uavcan_protocol_AccessCommandShellResponse_encode(uavcan_protocol_AccessCommandShellResponse* source, void* msg_buf)
{
    uint32_t offset = 0;

    offset = uavcan_protocol_AccessCommandShellResponse_encode_internal(source, msg_buf, offset, 1);

    return (offset + 7 ) / 8;
}

/**
  * @brief uavcan_protocol_AccessCommandShellResponse_decode_internal
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_AccessCommandShellResponse dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @param offset: Call with 0, bit offset to msg storage
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_AccessCommandShellResponse_decode_internal(
  const CanardRxTransfer* transfer,
  uint16_t CANARD_MAYBE_UNUSED(payload_len),
  uavcan_protocol_AccessCommandShellResponse* dest,
  uint8_t** CANARD_MAYBE_UNUSED(dyn_arr_buf),
  int32_t offset)
{
    int32_t ret = 0;
    uint32_t c = 0;

    ret = canardDecodeScalar(transfer, offset, 32, true, (void*)&dest->last_exit_status);
    if (ret != 32)
    {
        goto uavcan_protocol_AccessCommandShellResponse_error_exit;
    }
    offset += 32;

    ret = canardDecodeScalar(transfer, offset, 8, false, (void*)&dest->flags);
    if (ret != 8)
    {
        goto uavcan_protocol_AccessCommandShellResponse_error_exit;
    }
    offset += 8;

    // Dynamic Array (output)
    //  - Last item in struct & Root item & (Array Size > 8 bit), tail array optimization
    if (payload_len)
    {
        //  - Calculate Array length from MSG length
        dest->output.len = ((payload_len * 8) - offset ) / 8; // 8 bit array item size
    }
    else
    {
        // - Array length 9 bits
        ret = canardDecodeScalar(transfer,
                                 offset,
                                 9,
                                 false,
                                 (void*)&dest->output.len); // 255
        if (ret != 9)
        {
            goto uavcan_protocol_AccessCommandShellResponse_error_exit;
        }
        offset += 9;
    }

    //  - Get Array
    if (dyn_arr_buf)
    {
        dest->output.data = (uint8_t*)*dyn_arr_buf;
    }

    for (c = 0; c < dest->output.len; c++)
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
                goto uavcan_protocol_AccessCommandShellResponse_error_exit;
            }
            *dyn_arr_buf = (uint8_t*)(((uint8_t*)*dyn_arr_buf) + 1);
        }
        offset += 8;
    }
    return offset;

uavcan_protocol_AccessCommandShellResponse_error_exit:
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
  * @brief uavcan_protocol_AccessCommandShellResponse_decode
  * @param transfer: Pointer to CanardRxTransfer transfer
  * @param payload_len: Payload message length
  * @param dest: Pointer to destination struct
  * @param dyn_arr_buf: NULL or Pointer to memory storage to be used for dynamic arrays
  *                     uavcan_protocol_AccessCommandShellResponse dyn memory will point to dyn_arr_buf memory.
  *                     NULL will ignore dynamic arrays decoding.
  * @retval offset or ERROR value if < 0
  */
int32_t uavcan_protocol_AccessCommandShellResponse_decode(const CanardRxTransfer* transfer,
  uint16_t payload_len,
  uavcan_protocol_AccessCommandShellResponse* dest,
  uint8_t** dyn_arr_buf)
{
    const int32_t offset = 0;
    int32_t ret = 0;

    // Clear the destination struct
    for (uint32_t c = 0; c < sizeof(uavcan_protocol_AccessCommandShellResponse); c++)
    {
        ((uint8_t*)dest)[c] = 0x00;
    }

    ret = uavcan_protocol_AccessCommandShellResponse_decode_internal(transfer, payload_len, dest, dyn_arr_buf, offset);

    return ret;
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __UAVCAN_PROTOCOL_ACCESSCOMMANDSHELL