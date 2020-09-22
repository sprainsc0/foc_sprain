#pragma once


#include "ipc.h"

struct foc_status_s {
	uint64_t timestamp;
	uint8_t ctrl_mode;
	float phase_rad;
	float i_phase[3];
	float i_alpha;
	float i_beta;
	float i_q;
	float i_d;
	float v_d;
	float v_q;
	float mod_d;
	float mod_q;
	uint32_t svm_sector;
	float v_phase[3];
	float v_alpha;
	float v_beta;
	uint32_t pwm[3];
	float temperature;
	float vbus;
	float duty;
};

/* register this as object request broker structure */
IPC_DECLARE(foc_status);

