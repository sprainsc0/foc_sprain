#pragma once


#include "ipc.h"


typedef enum
{
	MC_CTRL_IDLE     = 0,
	MC_CTRL_ENABLE   = 1 << 0,
	MC_CTRL_OVERRIDE = 1 << 1,
	MC_CTRL_CURRENT  = 1 << 2,
	MC_CTRL_DUTY     = 1 << 3,
	MC_CTRL_SPEED    = 1 << 4,
	MC_CTRL_POSITION = 1 << 5,
} foc_ctrl_mode_t;


struct foc_target_s {
	uint64_t timestamp;
	
	uint8_t ctrl_mode;
	float id_target;
	float iq_target;
	float vd_target;
	float vq_target;
	float target_spd;
	float target_rad;
	float target_duty;

	float phase_override;
};

/* register this as object request broker structure */
IPC_DECLARE(foc_target);

