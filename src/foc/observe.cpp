#include "observe.h"
#include <string>
#include "hrt_timer.h"
#include "debug.h"
#include "utils.h"

Observer::Observer(void)
{
    memset(&_obs_m, 0, sizeof(_obs_m));
}

Observer::~Observer(void)
{

}

bool Observer::init(void)
{
	_param_handles.observer_type_handle    = param_find("OBSERVER_TYPE");
	_param_handles.motor_r_handle          = param_find("MOTOR_R");
	_param_handles.motor_l_handle          = param_find("MOTOR_L");
	_param_handles.flux_linkage_handle     = param_find("FLUX_LINKAGE");

	_param_handles.l_current_max_handle           = param_find("CURRENT_MAX");
	_param_handles.foc_sat_comp_handle            = param_find("SAT_COMP");
	_param_handles.foc_temp_comp_handle           = param_find("TEMP_COMP");
	_param_handles.foc_temp_comp_base_temp_handle = param_find("TEMP_BASE");

	_param_handles.observer_gain_handle           = param_find("OBS_GAIN");
	_param_handles.observer_gain_slow_handle      = param_find("OBS_GAIN_SLOW");

	_params_sub     = ipc_subscibe(IPC_ID(parameter_update));

	return true;
}

void Observer::parameter_update(bool force)
{
	bool updated = force;
    if (!updated) {
		ipc_check(_params_sub, &updated);
	}
    if (updated) {
		parameter_update_s param_update;
		ipc_pull(IPC_ID(parameter_update), _params_sub, &param_update);

		param_get(_param_handles.observer_type_handle,    &_obs_cfg.observer_type);
		param_get(_param_handles.motor_r_handle,          &_obs_cfg.motor_r);
		param_get(_param_handles.motor_l_handle,          &_obs_cfg.motor_l);
		param_get(_param_handles.flux_linkage_handle,     &_obs_cfg.flux_linkage);

		param_get(_param_handles.l_current_max_handle,           &_obs_cfg.l_current_max);
		param_get(_param_handles.foc_sat_comp_handle,            &_obs_cfg.foc_sat_comp);
		param_get(_param_handles.foc_temp_comp_handle,           &_obs_cfg.foc_temp_comp);
		param_get(_param_handles.foc_temp_comp_base_temp_handle, &_obs_cfg.foc_temp_comp_base_temp);
		param_get(_param_handles.observer_gain_handle,           &_obs_cfg.observer_gain);
		param_get(_param_handles.observer_gain_slow_handle,      &_obs_cfg.observer_gain_slow);
    }
}

void Observer::observer_idle(float *phase)
{
	*phase = utils_fast_atan2(_obs_m.m_x2_prev + _obs_m.m_observer_x2, _obs_m.m_x1_prev + _obs_m.m_observer_x1);

	_obs_m.m_x1_prev = _obs_m.m_observer_x1;
	_obs_m.m_x2_prev = _obs_m.m_observer_x2;
}

void Observer::observer_update(float v_alpha, float v_beta, float i_alpha, float i_beta,
					 float dt, volatile float *phase, volatile foc_status_s *motor)
{
	const float L = (3.0 / 2.0) * _obs_cfg.motor_l;
	float R = (3.0 / 2.0) * _obs_cfg.motor_r;

	// Saturation compensation
	const float sign = (motor->i_q * motor->v_q) >= 0.0 ? 1.0 : -1.0;
	R -= R * sign * _obs_cfg.foc_sat_comp * (motor->i_abs_filter / _obs_cfg.l_current_max);

	// Temperature compensation
	const float t = motor->temp_motor;
	if (_obs_cfg.foc_temp_comp && t > -25.0) {
		R += R * 0.00386 * (t - _obs_cfg.foc_temp_comp_base_temp);
	}

	const float L_ia = L * i_alpha;
	const float L_ib = L * i_beta;
	const float R_ia = R * i_alpha;
	const float R_ib = R * i_beta;
	const float lambda_2 = SQ(_obs_cfg.flux_linkage);
	// TODO
	const float gamma_half = _obs_m.m_gamma_now * 0.5; 

	switch (_obs_cfg.observer_type) {
	case FOC_OBSERVER_ORTEGA_ORIGINAL: {
		float err = lambda_2 - (SQ(_obs_m.m_observer_x1 - L_ia) + SQ(_obs_m.m_observer_x2 - L_ib));
		float x1_dot = -R_ia + v_alpha + gamma_half * (_obs_m.m_observer_x1 - L_ia) * err;
		float x2_dot = -R_ib + v_beta + gamma_half * (_obs_m.m_observer_x2 - L_ib) * err;
		_obs_m.m_observer_x1 += x1_dot * dt;
		_obs_m.m_observer_x2 += x2_dot * dt;
	} break;

	case FOC_OBSERVER_ORTEGA_ITERATIVE: {
		// Iterative with some trial and error
		const int iterations = 6;
		const float dt_iteration = dt / (float)iterations;
		for (int i = 0;i < iterations;i++) {
			float err = lambda_2 - (SQ(_obs_m.m_observer_x1 - L_ia) + SQ(_obs_m.m_observer_x2 - L_ib));
			float gamma_tmp = gamma_half;
			if (utils_truncate_number_abs(&err, lambda_2 * 0.2)) {
				gamma_tmp *= 10.0;
			}
			float x1_dot = -R_ia + v_alpha + gamma_tmp * (_obs_m.m_observer_x1 - L_ia) * err;
			float x2_dot = -R_ib + v_beta + gamma_tmp * (_obs_m.m_observer_x2 - L_ib) * err;

			_obs_m.m_observer_x1 += x1_dot * dt_iteration;
			_obs_m.m_observer_x2 += x2_dot * dt_iteration;
		}
	} break;

	default:
		break;
	}

	// Same as iterative, but without iterations.
//	float err = lambda_2 - (SQ(_obs_m.m_observer_x1 - L_ia) + SQ(_obs_m.m_observer_x2 - L_ib));
//	float gamma_tmp = gamma_half;
//	if (utils_truncate_number_abs(&err, lambda_2 * 0.2)) {
//		gamma_tmp *= 10.0;
//	}
//	float x1_dot = -R_ia + v_alpha + gamma_tmp * (_obs_m.m_observer_x1 - L_ia) * err;
//	float x2_dot = -R_ib + v_beta + gamma_tmp * (_obs_m.m_observer_x2 - L_ib) * err;
//	_obs_m.m_observer_x1 += x1_dot * dt;
//	_obs_m.m_observer_x2 += x2_dot * dt;

	UTILS_NAN_ZERO(_obs_m.m_observer_x1);
	UTILS_NAN_ZERO(_obs_m.m_observer_x2);

	if (phase) {
		*phase = utils_fast_atan2(_obs_m.m_observer_x2 - L_ib, _obs_m.m_observer_x1 - L_ia);
	}
}

void Observer::observer_task(volatile foc_status_s *motor)
{
	parameter_update(false);

	float gamma_tmp = utils_map(fabsf(motor->duty),
								0.0, 40.0 / motor->vbus,
								0,
								_obs_cfg.observer_gain);
	if (gamma_tmp < (_obs_cfg.observer_gain_slow * _obs_cfg.observer_gain)) {
		gamma_tmp = _obs_cfg.observer_gain_slow *  _obs_cfg.observer_gain;
	}

	// 4.0 scaling is kind of arbitrary, but it should make configs from old VESC Tools more likely to work.
	_obs_m.m_gamma_now = gamma_tmp * 4.0;
}

