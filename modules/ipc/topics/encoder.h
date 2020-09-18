
#pragma once


#include "ipc.h"


typedef enum
{
	ENC_ANGLE = 0,  		// magnet or potentialmeter (curve maybe linear or un-linear)
	ENC_ABZ,		 		// 2 pulse to indicate rotating increasement, Z is zero phase
	ENC_LINEAR_HALL, 		// 2 or 3 hall sensors detecting rotor magnetic
} enc_sensor_type_t;


struct encoder_s {
	uint64_t timestamp; // required for logger

	enc_sensor_type_t tpye;
	bool healthy;

	float angle;
	float angle_e;
	float angle_m;

#ifdef __cplusplus

#endif
};

/* register this as object request broker structure */
IPC_DECLARE(encoder);

