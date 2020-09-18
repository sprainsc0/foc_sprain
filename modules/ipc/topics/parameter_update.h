
#pragma once

#include "ipc.h"

#define PARAM_UPDATE_CALIBRATED_ALL            0
#define PARAM_UPDATE_CALIBRATED_ACCEL          1
#define PARAM_UPDATE_CALIBRATED_MAG            2
#define PARAM_UPDATE_CALIBRATED_BARO           3
#define PARAM_UPDATE_CALIBRATED_RC             4
#define PARAM_UPDATE_CALIBRATED_GYRO           5

struct parameter_update_s {
	uint64_t timestamp; // required for logger
	uint32_t instance;
    uint8_t update_mode;

#ifdef __cplusplus

#endif
};

/* register this as object request broker structure */
IPC_DECLARE(parameter_update);

