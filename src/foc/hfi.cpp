#include "hfi.h"
#include <string>
#include "hrt_timer.h"
#include "debug.h"
#include "utils.h"

const osThreadAttr_t hfi_attributes = {
		.name = "hfi",
		.priority = (osPriority_t)osPriorityRealtime1,
		.stack_size = 512};

static void hfi_func(HFI *pThis)
{
    pThis->run(pThis->_param);
}

HFI::HFI(void):
    _mod_alpha_tmp(0),
    _mod_beta_tmp(0),
    _param(NULL)
{
    memset(&_hfi_m, 0, sizeof(_hfi_m));
}

bool HFI::init(void *param)
{    
    _param_handles.foc_sample_v0_v7_handle = param_find("SAM_MODE");
    _param_handles.motor_r_handle          = param_find("MOTOR_R");
	_param_handles.motor_l_handle          = param_find("MOTOR_L");
    _param_handles.motor_l_diff_handle     = param_find("MOTOR_L_DIFF");
	_param_handles.flux_linkage_handle     = param_find("FLUX_LINKAGE");
    _param_handles.l_current_max_handle    = param_find("CURRENT_MAX");

    _param_handles.hfi_voltage_start_handle = param_find("HFI_V_START");
	_param_handles.hfi_voltage_run_handle   = param_find("HFI_V_RUN");
    _param_handles.hfi_voltage_max_handle   = param_find("HFI_V_MAX");
	_param_handles.hfi_start_samples_handle = param_find("HFI_START_SAM");
    _param_handles.hfi_obs_ovr_sec_handle   = param_find("HFI_OBS_OVER");
    _param_handles.hfi_samples_handle       = param_find("HFI_SAM_NUM");

    _params_sub     = ipc_subscibe(IPC_ID(parameter_update));
    _commander_sub  = ipc_subscibe(IPC_ID(foc_command));

    hfi_init(HFI_SAMPLES_16);

    _handle = osThreadNew((osThreadFunc_t)hfi_func, this, &hfi_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void HFI::parameter_update(bool force)
{
    bool updated = force;
    if (!updated) {
		ipc_check(_params_sub, &updated);
	}
    if (updated) {
		parameter_update_s param_update;
		ipc_pull(IPC_ID(parameter_update), _params_sub, &param_update);

        param_get(_param_handles.foc_sample_v0_v7_handle, &_hfi_cfg.foc_sample_v0_v7);
		param_get(_param_handles.motor_r_handle,          &_hfi_cfg.motor_r);
		param_get(_param_handles.motor_l_handle,          &_hfi_cfg.motor_l);
        param_get(_param_handles.motor_l_diff_handle,     &_hfi_cfg.motor_l_diff);
		param_get(_param_handles.flux_linkage_handle,     &_hfi_cfg.flux_linkage);
        param_get(_param_handles.l_current_max_handle,    &_hfi_cfg.l_current_max);

        param_get(_param_handles.hfi_voltage_start_handle, &_hfi_cfg.hfi_voltage_start);
		param_get(_param_handles.hfi_voltage_run_handle,   &_hfi_cfg.hfi_voltage_run);
        param_get(_param_handles.hfi_voltage_max_handle,   &_hfi_cfg.hfi_voltage_max);
		param_get(_param_handles.hfi_start_samples_handle, &_hfi_cfg.hfi_start_samples);
        param_get(_param_handles.hfi_obs_ovr_sec_handle,   &_hfi_cfg.hfi_obs_ovr_sec);
        param_get(_param_handles.hfi_samples_handle,       &_hfi_cfg.hfi_samples);
    }
}

void HFI::hfi_init(foc_hfi_samples samples)
{
    memset(&_hfi_m, 0, sizeof(_hfi_m));

    switch (samples) {
	case HFI_SAMPLES_8:
		_hfi_m.samples = 8;
		_hfi_m.table_fact = 4;
		_hfi_m.fft_bin0_func = utils_fft8_bin0;
		_hfi_m.fft_bin1_func = utils_fft8_bin1;
		_hfi_m.fft_bin2_func = utils_fft8_bin2;
		break;

	case HFI_SAMPLES_16:
		_hfi_m.samples = 16;
		_hfi_m.table_fact = 2;
		_hfi_m.fft_bin0_func = utils_fft16_bin0;
		_hfi_m.fft_bin1_func = utils_fft16_bin1;
		_hfi_m.fft_bin2_func = utils_fft16_bin2;
		break;

	case HFI_SAMPLES_32:
		_hfi_m.samples = 32;
		_hfi_m.table_fact = 1;
		_hfi_m.fft_bin0_func = utils_fft32_bin0;
		_hfi_m.fft_bin1_func = utils_fft32_bin1;
		_hfi_m.fft_bin2_func = utils_fft32_bin2;
		break;
	}
}

void HFI::hfi_sample(bool ready, float mod_alpha, float mod_beta, volatile foc_status_s *motor)
{
    if(!ready) {
        _hfi_m.ind         = 0;
		_hfi_m.ready       = false;
		_hfi_m.is_samp_n   = false;
		_hfi_m.prev_sample = 0.0f;

        return;
    }

    _mod_alpha_tmp = mod_alpha;
    _mod_beta_tmp = mod_beta;

    float hfi_voltage;
    if (_hfi_m.est_done_cnt < _hfi_cfg.hfi_start_samples) {
        hfi_voltage = _hfi_cfg.hfi_voltage_start;
    } else {
        hfi_voltage = utils_map(fabsf(motor->i_q), 0.0f, _hfi_cfg.l_current_max,
                                _hfi_cfg.hfi_voltage_run, _hfi_cfg.hfi_voltage_max);
    }

    utils_truncate_number_abs(&hfi_voltage, motor->vbus * (2.0f / 3.0f) * 0.9f);

    if (_hfi_m.is_samp_n) {
        float sample_now = (utils_tab_sin_32_1[_hfi_m.ind *_hfi_m.table_fact] * motor->i_alpha -
                utils_tab_cos_32_1[_hfi_m.ind * _hfi_m.table_fact] * motor->i_beta);
        float current_sample = sample_now - _hfi_m.prev_sample;

        _hfi_m.buffer_current[_hfi_m.ind] = current_sample;

        if (current_sample > 0.01f) {
            _hfi_m.buffer[_hfi_m.ind] = ((hfi_voltage / 2.0f - _hfi_cfg.motor_r * current_sample) / (_foc_f_sw * current_sample));
        }

        _hfi_m.ind++;
        if (_hfi_m.ind == _hfi_m.samples) {
            _hfi_m.ind = 0;
            _hfi_m.ready = true;
        }

        _mod_alpha_tmp += hfi_voltage * utils_tab_sin_32_1[_hfi_m.ind * _hfi_m.table_fact] / ((2.0f / 3.0f) * motor->vbus);
        _mod_beta_tmp -= hfi_voltage * utils_tab_cos_32_1[_hfi_m.ind * _hfi_m.table_fact] / ((2.0f / 3.0f) * motor->vbus);
    } else {
        _hfi_m.prev_sample = utils_tab_sin_32_1[_hfi_m.ind * _hfi_m.table_fact] * motor->i_alpha -
                utils_tab_cos_32_1[_hfi_m.ind * _hfi_m.table_fact] * motor->i_beta;

        _mod_alpha_tmp -= hfi_voltage * utils_tab_sin_32_1[_hfi_m.ind * _hfi_m.table_fact] / ((2.0f / 3.0f) * motor->vbus);
        _mod_beta_tmp += hfi_voltage * utils_tab_cos_32_1[_hfi_m.ind * _hfi_m.table_fact] / ((2.0f / 3.0f) * motor->vbus);
    }

    utils_saturate_vector_2d(&_mod_alpha_tmp, &_mod_beta_tmp, SQRT3_BY_2 * 0.95f);
	_hfi_m.is_samp_n = !_hfi_m.is_samp_n;
}

void HFI::hfi_idle(void)
{
    struct foc_status_s *motor_statue = (struct foc_status_s *)_param;

    _hfi_m.ind = 0;
    _hfi_m.ready = false;
    _hfi_m.is_samp_n = false;
    _hfi_m.prev_sample = 0.0;
    _hfi_m.angle = motor_statue->phase_rad;
}

void HFI::hfi_update(void)
{
    struct foc_status_s *motor_statue = (struct foc_status_s *)_param;

    if(_hfi_m.ready) {
        float real_bin1, imag_bin1, real_bin2, imag_bin2;
		_hfi_m.fft_bin1_func((float*)_hfi_m.buffer, &real_bin1, &imag_bin1);
		_hfi_m.fft_bin2_func((float*)_hfi_m.buffer, &real_bin2, &imag_bin2);

        float mag_bin_1 = sqrtf(SQ(imag_bin1) + SQ(real_bin1));
		float angle_bin_1 = -utils_fast_atan2(imag_bin1, real_bin1);

		angle_bin_1 += M_PI / 1.7f; // Why 1.7??
		angle_bin_1 = wrap_PI(angle_bin_1);

        float mag_bin_2 = sqrtf(SQ(imag_bin2) + SQ(real_bin2));
		float angle_bin_2 = -utils_fast_atan2(imag_bin2, real_bin2) / 2.0;

        // Assuming this thread is much faster than it takes to fill the HFI buffer completely,
		// we should lag 1/2 HFI buffer behind in phase. Compensate for that here.
		float dt_sw;
		if (_hfi_cfg.foc_sample_v0_v7) {
			dt_sw = 1.0 / _foc_f_sw;
		} else {
			dt_sw = 1.0 / (_foc_f_sw / 2.0f);
		}
		angle_bin_2 += motor_statue->speed_rad * ((float)_hfi_m.samples / 2.0f) * dt_sw;

        if (fabsf(angle_difference(angle_bin_2 + M_PI, _hfi_m.angle)) < fabsf(angle_difference(angle_bin_2, _hfi_m.angle))) {
			angle_bin_2 += M_PI;
		}

        if (_hfi_m.est_done_cnt < _hfi_cfg.hfi_start_samples) {
			_hfi_m.est_done_cnt++;

			if (fabsf(angle_difference(angle_bin_2, angle_bin_1)) > (M_PI / 2.0f)) {
				_hfi_m.flip_cnt++;
			}
		} else {
			if (_hfi_m.flip_cnt >= (_hfi_cfg.hfi_start_samples / 2)) {
				angle_bin_2 += M_PI;
			}
			_hfi_m.flip_cnt = 0;
		}

		_hfi_m.angle = angle_bin_2;

        _hfi_m.angle = wrap_PI(_hfi_m.angle);
    } else {
        _hfi_m.angle = motor_statue->phase_observer;
    }
}

void HFI::run(void *parameter)
{
    parameter_update(true);

    while (1)
    {
        parameter_update(false);

        bool cmd_updated = false;
        ipc_check(_commander_sub, &cmd_updated);
        if (cmd_updated) {
            struct foc_command_s command;
            ipc_pull(IPC_ID(foc_command), _commander_sub, &command);
            if(command.command == CMD_HFI_COMMAND) {

            }
        }

        hfi_update();
        // 100Hz loop
        osDelay(1);
    }
}

