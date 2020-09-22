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

bool RES_Cal::do_calibration()
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
    
    _foc_ref.ctrl_mode = MC_CTRL_IDLE;
    _foc_ref.id_target = 0;
    _foc_ref.iq_target = 0;
    _foc_ref.vd_target = 0;
    _foc_ref.vq_target = 0;
    _foc_ref.phase_override = 0;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);

    osDelay(2000);

    

    set_res_parameter();

    send_status(RES_CALIBRATE_SUCCESS);

    Info_Debug("[motor]Resistance Detected Success\n");
    
    return true;
}
