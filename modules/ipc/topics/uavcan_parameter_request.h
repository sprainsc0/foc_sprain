#pragma once

#include "ipc.h"

struct uavcan_parameter_request_s {
	uint64_t timestamp; // required for logger
	int64_t int_value;
	float real_value;
	int16_t param_index;
	uint8_t message_type;
	uint8_t node_id;
	char param_id[17];
	uint8_t param_type;
};

/* register this as object request broker structure */
IPC_DECLARE(uavcan_parameter_request);

