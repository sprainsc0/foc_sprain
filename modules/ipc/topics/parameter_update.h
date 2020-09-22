
#pragma once

#include "ipc.h"

#define PARAM_UPDATE_CALIBRATED_ALL            0
#define PARAM_UPDATE_CALIBRATED_EANG           1
#define PARAM_UPDATE_CALIBRATED_MANG           2
#define PARAM_UPDATE_CALIBRATED_RES            3

struct parameter_update_s {
	uint64_t timestamp; // required for logger
	uint32_t instance;
    uint8_t update_mode;

#ifdef __cplusplus

#endif
};

/* register this as object request broker structure */
IPC_DECLARE(parameter_update);

