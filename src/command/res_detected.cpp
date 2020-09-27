#include "res_detected.h"
#include "configure.h"
#include "cmsis_os.h"
#include "utils.h"
#include <string>
#include <hrt_timer.h>
#include <debug.h>

RES_Cal::RES_Cal(orb_advert_t &cal_status_pub):
    _mavlink_cal_pub(cal_status_pub),
    foc_mode(0)
{
    _cal_params_handles.motor_res_handle = param_find("MOTOR_R");

    _params_sub     = ipc_subscibe(IPC_ID(parameter_update));
    _foc_status_sub = ipc_subscibe(IPC_ID(foc_status));
    _foc_ref_pub    = ipc_active(IPC_ID(foc_target), &_foc_ref);
}

RES_Cal::~RES_Cal()
{
    ipc_unsubscibe(_params_sub);
    ipc_inactive(_foc_ref_pub);
}

void RES_Cal::set_res_parameter(void)
{
    param_set_no_notification(_cal_params_handles.motor_res_handle,   &_res_calibration.motor_res);

    param_notify_changes(PARAM_UPDATE_CALIBRATED_MOTOR);
}

void RES_Cal::send_status(uint8_t status)
{
    if(_mavlink_cal_pub == nullptr) {
        _mavlink_cal_pub = ipc_active(IPC_ID(calibrate_status), &_cal_status);
    }
    _cal_status.calibrate_type = RES_CALIBRATING;

    _cal_status.calibrate_statu = status;
    ipc_push(IPC_ID(calibrate_status), _mavlink_cal_pub, &_cal_status);
}

bool RES_Cal::do_calibration(float param)
{
    bool updated = false;
    Info_Debug("[motor]Resistance Detected Waiting FOC Status\n");
    while (1) {
        ipc_check(_foc_status_sub, &updated);
        if(updated) {
            struct foc_status_s data;
            ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);
            foc_mode = data.ctrl_mode;
            break;
        }
    }
    Info_Debug("[motor]Resistance Detected start\n");

    send_status(RES_CALIBRATE_STARTED);
    
    _foc_ref.ctrl_mode = (MC_CTRL_OVERRIDE | MC_CTRL_CURRENT | MC_CTRL_ENABLE);
    _foc_ref.id_target = 0;
    _foc_ref.iq_target = 0;
    _foc_ref.vd_target = 0;
    _foc_ref.vq_target = 0;
    _foc_ref.phase_override = 0;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);

    osDelay(200);

    for(int c = 0; _foc_ref.iq_target <= param; c++) {
        _foc_ref.iq_target += 0.005f;
        ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
        osDelay(10);
    }

    osDelay(100);

    // sample
    _sample.avg_current_tot = 0.0f;
	_sample.avg_voltage_tot = 0.0f;
	_sample.sample_num = 0;

    int cnt = 0;
	while (_sample.sample_num < 200) {
		osDelay(1);
		cnt++;
		// Timeout
		if (cnt > 10000) {
            _foc_ref.ctrl_mode = MC_CTRL_IDLE;
            _foc_ref.id_target = 0;
            _foc_ref.iq_target = 0;
            _foc_ref.vd_target = 0;
            _foc_ref.vq_target = 0;
            _foc_ref.phase_override = 0;
            ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
            send_status(RES_CALIBRATE_FAILED);
            Info_Debug("[motor]Resistance Detected Failed\n");
			break;
		}
        updated = false;
        ipc_check(_foc_status_sub, &updated);
        if(updated) {
            struct foc_status_s data;
            ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);

            _sample.avg_current_tot += sqrtf(SQ(data.i_d) + SQ(data.i_q));
            _sample.avg_voltage_tot += sqrtf(SQ(data.v_d) + SQ(data.v_q));
            _sample.sample_num++;
        }
	}

    const float current_avg = _sample.avg_current_tot / (float)_sample.sample_num;
	const float voltage_avg = _sample.avg_voltage_tot / (float)_sample.sample_num;

    _foc_ref.ctrl_mode = MC_CTRL_IDLE;
    _foc_ref.id_target = 0;
    _foc_ref.iq_target = 0;
    _foc_ref.vd_target = 0;
    _foc_ref.vq_target = 0;
    _foc_ref.phase_override = 0;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);

    _res_calibration.motor_res = (voltage_avg / current_avg) * (2.0f / 3.0f);

    set_res_parameter();

    send_status(RES_CALIBRATE_SUCCESS);

    Info_Debug("[motor]Resistance Detected Success\n");
    
    return true;
}
