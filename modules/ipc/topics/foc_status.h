#pragma once


#include "ipc.h"

typedef enum
{
	MC_SENSOR_ENC = 0,
	MC_SENSOR_HAL = 1,
	MC_SENSOR_HFI = 2,
	MC_SENSORLESS = 3
} foc_sensor_type_t;

typedef enum {
	FOC_CC_DECOUPLING_DISABLED = 0,
	FOC_CC_DECOUPLING_CROSS,
	FOC_CC_DECOUPLING_BEMF,
	FOC_CC_DECOUPLING_CROSS_BEMF
} mc_foc_cc_decoupling_type;

struct foc_status_s {
	uint64_t timestamp;
	uint8_t ctrl_mode;
	float phase_rad;
	float phase_rad_filter;
	float phase_observer;
	float speed_rad;
	float i_phase[3];
	float i_alpha;
	float i_beta;
	float i_q;
	float i_d;
	float i_q_filter;
	float i_d_filter;
	float v_d;
	float v_q;
	float mod_d;
	float mod_q;
	uint32_t svm_sector;
	float v_phase[3];
	float v_alpha;
	float v_beta;
	uint32_t pwm[3];
	float temp_chip;
	float temp_pcb;
	float temp_motor;
	float vbus;
	float ibus;
	float duty;
	float i_abs_filter;
};

/* register this as object request broker structure */
IPC_DECLARE(foc_status);

