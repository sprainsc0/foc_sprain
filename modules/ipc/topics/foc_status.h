#pragma once


#include "ipc.h"


typedef enum
{
	MC_CTRL_IDLE = 0,
	MC_CTRL_ENABLE = 1 << 0,
	MC_CTRL_OVERRIDE = 1 << 1,
	MC_CTRL_CURRENT = 1 << 2,
	MC_CTRL_SPEED = 1 << 3,
	MC_CTRL_POSITION = 1 << 4,
	MC_CTRL_TEST = 1 << 5,
} foc_ctrl_mode_t;


struct foc_status_s {
	uint64_t timestamp;
	
	uint8_t ctrl_mode;
	float i_phase[3];
	float i_alpha;
	float i_beta;
	float i_q;
	float i_d;
};

/* register this as object request broker structure */
IPC_DECLARE(foc_status);

