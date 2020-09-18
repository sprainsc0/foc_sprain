#pragma once

#include "ipc.h"

struct uavcan_command_request_s {
	uint64_t timestamp;                       // required for logger
	uint8_t    cmd;                           // bit len 8
	uint8_t    sub_cmd;
    float      param1;                        // float16 Saturate
    float      param2;                        // float16 Saturate
	uint8_t    node_id;
	uint8_t    cmd_type;
};

/* register this as object request broker structure */
IPC_DECLARE(uavcan_command_request);

