#include "elec_calibration.h"
#include "configure.h"
#include "cmsis_os.h"
#include "utils.h"
#include <string>
#include <hrt_timer.h>
#include <debug.h>

Enc_CalE::Enc_CalE(orb_advert_t &cal_status_pub):
    _mavlink_cal_pub(cal_status_pub),
    foc_mode(0)
{
    _cal_params_handles.offset_e_handle = param_find("ENC_E_OFFSET");
    _cal_params_handles.invert_e_handle = param_find("ENC_E_INVERT");
    // _cal_params_handles.pair_num_handle = param_find("PAIR_NUM");

    _params_sub     = ipc_subscibe(IPC_ID(parameter_update));
    _enc_sub        = ipc_subscibe(IPC_ID(encoder));
    _foc_status_sub = ipc_subscibe(IPC_ID(foc_status));
    _foc_ref_pub    = ipc_active(IPC_ID(foc_target), &_foc_ref);
}

Enc_CalE::~Enc_CalE()
{
    ipc_unsubscibe(_params_sub);
    ipc_unsubscibe(_enc_sub);
    ipc_inactive(_foc_ref_pub);
}

void Enc_CalE::set_enc_parameter(void)
{
    param_set_no_notification(_cal_params_handles.offset_e_handle,   &_enc_calibration.offset_e);
    param_set_no_notification(_cal_params_handles.invert_e_handle,   &_enc_calibration.invert_e);
    // param_set_no_notification(_cal_params_handles.pair_num_handle,   &_enc_calibration.pair_num);

    param_notify_changes(PARAM_UPDATE_CALIBRATED_EANG);
}

void Enc_CalE::send_status(uint8_t status)
{
    if(_mavlink_cal_pub == nullptr) {
        _mavlink_cal_pub = ipc_active(IPC_ID(calibrate_status), &_cal_status);
    }
    _cal_status.calibrate_type = ENCE_CALIBRATING;

    _cal_status.calibrate_statu = status;
    ipc_push(IPC_ID(calibrate_status), _mavlink_cal_pub, &_cal_status);
}

bool Enc_CalE::do_calibration(float param, uint8_t use_current)
{
    float *d_axis;
    float raw_angle;

    bool updated = false;
    Info_Debug("[encoder]Encoder Electronic Waiting FOC Status\n");
    while (1) {
        ipc_check(_foc_status_sub, &updated);
        if(updated) {
            struct foc_status_s data;
            ipc_pull(IPC_ID(foc_status), _foc_status_sub, &data);
            foc_mode = data.ctrl_mode;
            break;
        }
    }
    Info_Debug("[encoder]Encoder Electronic Calibration start\n");
    send_status(ENC_CALIBRATE_STARTED);
    
    if (use_current) {
		_foc_ref.ctrl_mode = (MC_CTRL_OVERRIDE | MC_CTRL_CURRENT | MC_CTRL_ENABLE);
        _foc_ref.id_target = 0;
        _foc_ref.iq_target = 0;
        _foc_ref.vd_target = 0;
        _foc_ref.vq_target = 0;
        _foc_ref.phase_override = 0 - M_PI / 2;
		d_axis = &_foc_ref.id_target;
	} else {
		_foc_ref.ctrl_mode = (MC_CTRL_OVERRIDE | MC_CTRL_ENABLE);
		_foc_ref.id_target = 0;
        _foc_ref.iq_target = 0;
        _foc_ref.vd_target = 0;
        _foc_ref.vq_target = 0;
        _foc_ref.phase_override = 0 - M_PI / 2;
		d_axis = &_foc_ref.vd_target;
	}

    for(int c = 0; *d_axis <= param; c++) {
        *d_axis += 0.005f;
        ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
        osDelay(10);
    }

    // wait encoder stable
    if(!enc_stable_waite(ENC_ELEC_STABLE_VAR_TOR)) {
        send_status(ENC_CALIBRATE_FAILED);
        Info_Debug("[encoder]Encoder Electronic Calibration Failed(Motion1)\n");
        _foc_ref.ctrl_mode = MC_CTRL_IDLE;
        ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
        return false;
    }

    raw_angle = collect_sample();

    for (; _foc_ref.phase_override < 0; _foc_ref.phase_override += 1.7e-4f) {
        ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
		osDelay(1);
    }
	if(!enc_stable_waite(ENC_ELEC_STABLE_VAR_TOR)) {
        send_status(ENC_CALIBRATE_FAILED);
        Info_Debug("[encoder]Encoder Electronic Calibration Failed(Motion2)\n");
        _foc_ref.ctrl_mode = MC_CTRL_IDLE;
        ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);
        return false;
    }

	if (angle_difference(collect_sample(), raw_angle) > 0.0f) {
		_enc_calibration.invert_e = 0;
    } else {
		_enc_calibration.invert_e = 1;
    }
	osDelay(10);
    _enc_calibration.offset_e = collect_sample();

    set_enc_parameter();

    send_status(ENC_CALIBRATE_SUCCESS);

    _foc_ref.ctrl_mode = MC_CTRL_IDLE;
    ipc_push(IPC_ID(foc_target), _foc_ref_pub, &_foc_ref);

    Info_Debug("[encoder]Encoder Electronic Calibration Success\n");
    
    return true;
}

#define ANGLE_BUFFER_SIZE	256
bool Enc_CalE::enc_stable_waite(float var)
{
	float buffer[ANGLE_BUFFER_SIZE];
	uint32_t i = 0;
	bool start = false;

	memset(buffer, 0, sizeof buffer);
    const uint32_t start_ts = millis();

	while (1) {
		buffer[i++] = collect_sample();
		if (i >= ANGLE_BUFFER_SIZE) {
			i = 0;
			start = true;
		}

		if (start && angles_variance(buffer, ANGLE_BUFFER_SIZE) < var) {
			return true;
        } else if(start) {
            i = 0;
            start = false;
            send_status(ENC_CALIBRATE_MOTION);
            Info_Debug("[encoder]Encoder Electronic Calibration Detect Motion\n");
        }

        if(millis() - start_ts > 20000) {
            return false;
        }

		// oscilate rate is about 20Hz, so sample rate is about x4.
		osDelay(10);
	}
}

float Enc_CalE::collect_sample(void)
{
    struct encoder_s enc_raw;

    _sample_buffer.sample_count = 0;
    _sample_buffer.ang_rad = 0.0f;

    while(_sample_buffer.sample_count < 2) {
        bool updated = false;
        ipc_check(_enc_sub, &updated);
        if(updated) {
            ipc_pull(IPC_ID(encoder), _enc_sub, &enc_raw);
            _sample_buffer.ang_rad += enc_raw.angle;
            _sample_buffer.sample_count++;
        }
        osDelay(2);
    }
    return (_sample_buffer.ang_rad / _sample_buffer.sample_count);
}

