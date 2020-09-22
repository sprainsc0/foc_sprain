#pragma once


#include <ipc.h>

#ifndef __cplusplus

#endif

#define ENCE_CALIBRATING                0u
#define ENCM_CALIBRATING                1u
#define RES_CALIBRATING                 2u
#define IND_CALIBRATING                 3u
#define FLUX_CALIBRATING                4u

enum ENC_CALIBRATE_STATUS {
	ENC_CALIBRATE_IDEL = 0,
	ENC_CALIBRATE_STARTED,
	ENC_CALIBRATE_SUCCESS,
	ENC_CALIBRATE_MOTION,
	ENC_CALIBRATE_FAILED
};

enum RES_CALIBRATE_STATUS {
	RES_CALIBRATE_IDEL = 0,
	RES_CALIBRATE_STARTED,
	RES_CALIBRATE_SUCCESS,
	RES_CALIBRATE_FAILED
};

enum IND_CALIBRATE_STATUS {
	IND_CALIBRATE_IDEL = 0,
	IND_CALIBRATE_STARTED,
	IND_CALIBRATE_SUCCESS,
	IND_CALIBRATE_FAILED
};

struct calibrate_status_s {
	uint64_t timestamp;
	uint8_t calibrate_type;
	uint8_t calibrate_statu;
	float param1;
	float param2;
	float param3;
};

/* register this as object request broker structure */
IPC_DECLARE(calibrate_status);

