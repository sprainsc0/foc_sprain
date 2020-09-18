#include "platform.h"
#include "encoder.h"
#include "uavcan.h"
#include "mc.h"
#include "utils.h"

extern float command_param;
extern void mc_ctrl_torque_write(int16_t t);

void user_task(void *argument)
{
	while(1)
	{
		switch(mc.mode) {
		case MC_MODE_IDLE:
			osDelay(100);
			mc_ctrl_torque_write(0);
			break;
		case MC_MODE_TEST:
			mc_ctrl_torque_write((int16_t)command_param);
			osDelay(100);
			break;
		case MC_MODE_CALIBRATE_ELEC:
			enc_elec_calibrate(command_param);
			commandack(MC_MODE_CALIBRATE_ELEC, mc_cfg.encoder.elec_offset, mc_cfg.encoder.elec_invert);
			mc.mode = MC_MODE_STORE_SETTING;
			break;
		case MC_MODE_CALIBRATE_MECH:
			enc_mech_calibrate();
			commandack(MC_MODE_CALIBRATE_MECH, mc_cfg.encoder.mech_offset, mc_cfg.encoder.mech_invert);
			mc.mode = MC_MODE_STORE_SETTING;
			break;
		case MC_MODE_RESET_SETTING:
			mc_param_load_default();
			commandack(MC_MODE_RESET_SETTING, 0.0f, 0.0f);
			mc.mode = MC_MODE_STORE_SETTING;
			break;
		case MC_MODE_STORE_SETTING:
			mc_param_save();
			commandack(MC_MODE_STORE_SETTING, 0.0f, 0.0f);
			mc.mode = MC_MODE_IDLE;
			break;
		case MC_MODE_GIMBAL:
			gimbal_ctrl_process();
			break;
		case MC_MODE_REBOOT:
			__disable_irq();
			HAL_NVIC_SystemReset();
			break;
		default:
			break;
		}

		osDelay(10);
	}
}

#define ANGLE_BUFFER_SIZE	256
bool enc_stable_waite(float var)
{
	static float buffer[ANGLE_BUFFER_SIZE];
	uint32_t i = 0;
	bool start = false;

	memset(buffer, 0, sizeof buffer);

	while (1)
	{
		buffer[i++] = encoder.mech_angle;
		if (i >= ANGLE_BUFFER_SIZE)
		{
			i = 0;
			start = true;
		}

		if (start && angles_variance(buffer, ANGLE_BUFFER_SIZE) < var)
			return true;

		// oscilate rate is about 20Hz, so sample rate is about x4.
		osDelay(10);
	}
}

bool enc_elec_calibrate(float param)
{
	if(param <= 0.0f || param > 2.0f) {
		return false;
	}

	float *d;
	float raw_angle;
	bool dir_invert;
	float duty_max = mc.duty_max;

	mc.duty_max = 0.8;
	dir_invert = mc_cfg.encoder.dir_invert;
	
	if (mc.ctrl_loop & MC_CTRL_CURRENT)
	{
		mc_ctrl_loop_write(MC_CTRL_OVERRIDE | MC_CTRL_CURRENT | MC_CTRL_ENABLE);
		d = &mc.target.id;
	}
	else
	{
		mc_ctrl_loop_write(MC_CTRL_OVERRIDE | MC_CTRL_ENABLE);
		d = &mc.target.vd;
	}

	mc_cfg.encoder.elec_offset = 0;
	mc_cfg.encoder.elec_invert = false;
	mc_cfg.encoder.dir_invert = false;

	mc.target.phase = 0 - M_PI / 4;		// TODO: speed loop here
	for (*d = 0; *d <= param; *d += 0.01f)
		osDelay(10);
	enc_stable_waite(ENC_ELEC_STABLE_VAR_TOR);
	raw_angle = encoder.raw_angle;

	for (; mc.target.phase < 0; mc.target.phase += 0.001f)
		osDelay(1);
	enc_stable_waite(ENC_ELEC_STABLE_VAR_TOR);

	if (angle_difference(encoder.raw_angle, raw_angle) > 0.0f)
		mc_cfg.encoder.elec_invert = false;
	else
		mc_cfg.encoder.elec_invert = true;

	osDelay(10);

	mc_cfg.encoder.elec_offset = encoder.elec_angle;		// TODO
	mc_cfg.encoder.dir_invert = dir_invert;
	
	mc_ctrl_loop_write(MC_CTRL_IDLE);
	mc.duty_max = duty_max;
	
	return true;
}

bool enc_mech_calibrate(void)
{
	// speed has jitter
	mc_cfg.encoder.mech_offset = 0;
	osDelay(100);

	mc_ctrl_loop_write(MC_CTRL_IDLE);
	
	osDelay(2000);				// You got 2s to calibrate 3 axis by hand
	enc_stable_waite(ENC_MECH_STABLE_VAR_TOR);

	mc_cfg.encoder.mech_offset = encoder.mech_angle;

	return true;
}
