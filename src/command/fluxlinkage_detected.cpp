#include "fluxlinkage_detected.h"
#include "configure.h"
#include "cmsis_os.h"
#include "utils.h"
#include <string>
#include <hrt_timer.h>
#include <debug.h>

FLUX_Cal::FLUX_Cal(orb_advert_t &cal_status_pub):
    _mavlink_cal_pub(cal_status_pub),
    foc_mode(0)
{
    _cal_params_handles.motor_res_handle  = param_find("MOTOR_R");
    _cal_params_handles.motor_ind_handle  = param_find("MOTOR_L");
    _cal_params_handles.ind_diff_handle   = param_find("MOTOR_L_DIFF");
    _cal_params_handles.motor_flux_handle = param_find("FLUX_LINKAGE");
    _cal_params_handles.obs_gain_handle   = param_find("OBS_GAIN");

    _params_sub     = ipc_subscibe(IPC_ID(parameter_update));
    _foc_status_sub = ipc_subscibe(IPC_ID(foc_status));
    _foc_ref_pub    = ipc_active(IPC_ID(foc_target), &_foc_ref);
}

FLUX_Cal::~FLUX_Cal()
{
    ipc_unsubscibe(_params_sub);
    ipc_inactive(_foc_ref_pub);
}

void FLUX_Cal::set_flux_parameter(void)
{
    param_set_no_notification(_cal_params_handles.motor_flux_handle,  &_flux_calibration.motor_flux);
    param_set_no_notification(_cal_params_handles.obs_gain_handle,   &_flux_calibration.obs_gain);

    param_notify_changes(PARAM_UPDATE_CALIBRATED_MOTOR);
}

void FLUX_Cal::get_flux_parameter(void)
{
    param_get(_cal_params_handles.motor_res_handle,   &_flux_calibration.motor_res);
    param_get(_cal_params_handles.motor_ind_handle,   &_flux_calibration.motor_ind);
    param_get(_cal_params_handles.ind_diff_handle,    &_flux_calibration.ind_diff);
    param_get(_cal_params_handles.motor_flux_handle,  &_flux_calibration.motor_flux);
    param_get(_cal_params_handles.obs_gain_handle,    &_flux_calibration.obs_gain);
}

void FLUX_Cal::send_status(uint8_t status)
{
    if(_mavlink_cal_pub == nullptr) {
        _mavlink_cal_pub = ipc_active(IPC_ID(calibrate_status), &_cal_status);
    }
    _cal_status.calibrate_type = FLUX_CALIBRATING;

    _cal_status.calibrate_statu = status;
    ipc_push(IPC_ID(calibrate_status), _mavlink_cal_pub, &_cal_status);
}

void FLUX_Cal::foc_openloop(float current, float rpm)
{
    _foc_ref.ctrl_mode = (MC_CTRL_CURRENT | MC_CTRL_ENABLE | MC_CTRL_OPENLOOP);
    _foc_ref.id_target = 0;
    _foc_ref.iq_target = current;
    _foc_ref.vd_target = 0;
    _foc_ref.vq_target = 0;
    _foc_ref.phase_override = 0;
    _foc_ref.openloop_spd = rpm;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
}

bool FLUX_Cal::do_calibration(float curr, float duty)
{
    float linkage = 0.0f;
    bool updated = false;
    Info_Debug("[motor]Flux Linkage Detected Waiting FOC Status\n");
    while (1) {
        ipc_check(_foc_status_sub, &updated);
        if(updated) {
            struct foc_status_s data;
            ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);
            foc_mode = data.ctrl_mode;
            break;
        }
    }
    Info_Debug("[motor]Flux Linkage Detected start\n");

    send_status(FLUX_CALIBRATE_STARTED);

    get_flux_parameter();
    
    _foc_ref.ctrl_mode = MC_CTRL_IDLE;
    _foc_ref.id_target = 0;
    _foc_ref.iq_target = 0;
    _foc_ref.vd_target = 0;
    _foc_ref.vq_target = 0;
    _foc_ref.phase_override = 0;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);

    osDelay(1000);

    float rpm_now = 0.0f;
    for (int i = 0;i < 200;i++) {
		foc_openloop((float)i * curr / 200.0f, rpm_now);
		osDelay(1);
	}

    Info_Debug("[motor]Flux Linkage wait current stable\n");

    float duty_still = 0;
	float samples = 0;
    for (int i = 0;i < 1000;) {
        updated = false;
        ipc_check(_foc_status_sub, &updated);
        if(updated) {
            struct foc_status_s data;
            ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);
            duty_still += fabsf(data.duty);
		    samples += 1.0f;
            i++;
        }
		osDelay(1);
	}
    duty_still /= samples;

    float duty_max = 0.0;
	const int max_time = 20000;
    float duty_now = 0.0f;
    int cnt = 0;

    while(duty_now < duty) {
        rpm_now += 0.05f;
        foc_openloop(curr, rpm_now);
        osDelay(1);
        cnt++;

        struct foc_status_s data;
        ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);
        duty_now = fabsf(data.duty);

        if (duty_now > duty_max) {
			duty_max = duty_now;
		}

        if (cnt >= max_time) {
			linkage = -1.0f;
			break;
		}

        if (cnt > 4000 && duty_now < (duty_max * 0.7f)) {
			cnt = max_time;
			linkage = -2.0f;
			break;
		}

        if (cnt > 4000 && duty < duty_still * 1.1f) {
			cnt = max_time;
			linkage = -3.0f;
			break;
		}

        if (rpm_now >= 1200) {
			break;
		}
    }

    Info_Debug("[motor]Flux Linkage wait rpm stable\n");

    osDelay(1000);

    if(cnt < max_time) {
        float vq_avg = 0.0;
		float vd_avg = 0.0;
		float iq_avg = 0.0;
		float id_avg = 0.0;
		float samples2 = 0.0;

        for (int i = 0;i < 10000;i++) {
            struct foc_status_s data;
            ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);
			vq_avg += data.v_q;
			vd_avg += data.v_d;
			iq_avg += data.i_q;
			id_avg += data.i_d;
			samples2 += 1.0f;
			osDelay(1);
		}

        vq_avg /= samples2;
		vd_avg /= samples2;
		iq_avg /= samples2;
		id_avg /= samples2;

        float v_mag = sqrtf(SQ(vq_avg) + SQ(vd_avg));
		float i_mag = sqrtf(SQ(iq_avg) + SQ(id_avg));
		linkage = (v_mag - (2.0f / 3.0f) * _flux_calibration.motor_res * i_mag) / rpm_now - (2.0f / 3.0f) * i_mag * _flux_calibration.motor_ind;

        _flux_calibration.motor_flux = linkage;
        _flux_calibration.obs_gain   = 0.5e3f / SQ(linkage);

        set_flux_parameter();
    } else {
        _foc_ref.ctrl_mode = MC_CTRL_IDLE;
        ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
        send_status(FLUX_CALIBRATE_FAILED);
        Info_Debug("[motor]Flux Linkage Detected Failed %d\n", (int)linkage);

        return false;
    }
    _foc_ref.ctrl_mode = MC_CTRL_IDLE;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);

    send_status(FLUX_CALIBRATE_SUCCESS);
    char print[50];
    sprintf(print, "[motor]Flux Detected Success - %.6f", linkage);
    Info_Debug("%s \n", print);
    
    return true;
}
