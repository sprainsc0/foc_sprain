#include "tim.h"
#include "hrt_timer.h"
#include "debug.h"
#include "encoder.h"
#include "amt222.h"
#include "utils.h"
#include "foc_function.h"

const osThreadAttr_t enc_attributes = {
		.name = "enc",
		.priority = (osPriority_t)osPriorityHigh,
		.stack_size = 512};

namespace MC_ENC {
	static Encoder *gEnc;
}

static void TIM2_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    MC_ENC::gEnc->enc_process();
}

static void enc_func(Encoder *pThis)
{
    pThis->run(pThis->_param);
}

Encoder::Encoder(void) :
	_enc_ready(false),
	_param(NULL)
{

}

Encoder::~Encoder(void)
{

}

bool Encoder::init(void)
{
	_params_sub     = ipc_subscibe(IPC_ID(parameter_update));
	_encoder_pub    = ipc_active(IPC_ID(encoder), &_enc_data);

	_param_handles.type_handle     = param_find("ENC_TYPE");
	_param_handles.offset_e_handle = param_find("ENC_E_OFFSET");
	_param_handles.offset_m_handle = param_find("ENC_M_OFFSET");
	_param_handles.invert_e_handle = param_find("ENC_E_INVERT");
	_param_handles.invert_m_handle = param_find("ENC_M_INVERT");
	_param_handles.pair_num_handle = param_find("PAIR_NUM");

	enc_tim_int  = perf_alloc(PC_INTERVAL, "tim_int");
	enc_task_int = perf_alloc(PC_INTERVAL, "enc_int");

	enc_tim_ela  = perf_alloc(PC_ELAPSED, "tim_ela");
	enc_task_ela = perf_alloc(PC_ELAPSED, "enc_ela");

	enc_err_count = perf_alloc(PC_COUNT, "enc_err");

	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM2_PeriodElapsedCallback);

	// enc
	HAL_TIM_Base_Start_IT(&htim2);

	_handle = osThreadNew((osThreadFunc_t)enc_func, this, &enc_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void Encoder::parameter_update(bool force)
{
	bool updated = force;
    if (!updated) {
		ipc_check(_params_sub, &updated);
	}
    if (updated) {
		parameter_update_s param_update;
		ipc_pull(IPC_ID(parameter_update), _params_sub, &param_update);
        
		param_get(_param_handles.type_handle,     &_enc_cfg.type);
		param_get(_param_handles.offset_e_handle, &_enc_cfg.offset_e);
		param_get(_param_handles.offset_m_handle, &_enc_cfg.offset_m);
		param_get(_param_handles.invert_e_handle, &_enc_cfg.invert_e);
		param_get(_param_handles.invert_m_handle, &_enc_cfg.invert_m);
		param_get(_param_handles.pair_num_handle, &_enc_cfg.pair_num);
    }
}

void Encoder::run(void *parammeter)
{
	parameter_update(true);
	uint64_t start_timr = micros();

	while(1) {
		const uint64_t ts = micros();

		perf_begin(enc_task_ela);

		perf_count(enc_task_int);

		parameter_update(false);
		if(((ts - start_timr) > 250000) && !_enc_ready) {
			hal_amt222_reset();
			osDelay(250);
			_enc_ready = true;
		}

		perf_end(enc_task_ela);

		osDelay(2);
	}
}

void Encoder::enc_process(void)
{
	uint32_t raw;
	float elec_angle, raw_angle, mech_angle;
	bool healthy = false;

	if(!_enc_ready) {
		return;
	}

	perf_begin(enc_tim_ela);

	if ((_enc_cfg.type == ENC_ANGLE) && hal_amt222_read(&raw)) {
		perf_count(enc_tim_int);

		healthy = true;
		// rad
		raw_angle = (float)(raw / 16384.0f) * M_2PI;

		// eletric angle
		if (_enc_cfg.invert_e) {
			elec_angle = (M_2PI - raw_angle) * _enc_cfg.pair_num;
		} else {
			elec_angle = raw_angle * _enc_cfg.pair_num;
		}

		elec_angle -= _enc_cfg.offset_e;
		elec_angle = wrap_2PI(elec_angle);

		// position
		mech_angle = raw_angle - _enc_cfg.offset_m;
		if (_enc_cfg.invert_m) {
			mech_angle = M_2PI - mech_angle;
		}
		mech_angle = wrap_2PI(mech_angle);
	} else if (_enc_cfg.type == ENC_ABZ) {
		/* code */
	} else {
		perf_count(enc_err_count);
	}

	_enc_data.timestamp = micros();
	_enc_data.tpye      = (enc_sensor_type_t)_enc_cfg.type;
	_enc_data.healthy   = healthy;
	_enc_data.angle     = raw_angle;
	_enc_data.angle_e   = elec_angle;
	_enc_data.angle_m   = mech_angle;
	// push foc status
	ipc_push(IPC_ID(encoder), _encoder_pub, &_enc_data);

	perf_end(enc_tim_ela);
}

int enc_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {
			if (MC_ENC::gEnc != nullptr) {
                Info_Debug("already running\n");
                return 0;
            }

            MC_ENC::gEnc = new Encoder();
            

            if (MC_ENC::gEnc == NULL) {
                Info_Debug("alloc failed\n");
                return 0;
            }

            MC_ENC::gEnc->init();
        }
    }
    return 1;
}
