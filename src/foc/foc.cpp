#include <datatypes.h>
#include "hrt_timer.h"
#include <utils.h>
#include <platform.h>
#include "debug.h"
#include "foc.h"
#include "hfi.h"
#include "observe.h"
#include "tim.h"
#include "main.h"
#include "../encoder/encoder.h"
#include "foc_function.h"

static uint16_t adc_current[3];
static uint16_t adc_voltage[3];
static uint16_t adc_hall[3];
static uint16_t adc_msic[3];

const osThreadAttr_t foc_attributes = {
		.name = "foc",
		.priority = (osPriority_t)osPriorityRealtime2,
		.stack_size = 1024};

namespace MC_FOC {
#pragma default_variable_attributes = @ ".ccram"
	static FOC     gFOC;
	static Encoder gEnc;
	static HFI     gHfi;
	static Observe gObser;
#pragma default_variable_attributes =
}

bool FOC::_power_state = false;

static void ADC1_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    MC_FOC::gFOC.foc_process();
}

static void TIM2_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    MC_FOC::gEnc.enc_process();
}

static void foc_func(FOC *pThis)
{
    pThis->run(pThis->_param);
}

FOC::FOC(void):
	_id_ctrl(0.0f, 0.0f, 0.0f, 1.0f, 60.0f, CURRENT_RATE_DT),
	_iq_ctrl(0.0f, 0.0f, 0.0f, 1.0f, 60.0f, CURRENT_RATE_DT),
	_refint(0),
	_pre_foc_mode(0),
	_param(NULL)
{
	memset(&_encoder_data, 0, sizeof(_encoder_data));
	memset(&_foc_ref, 0, sizeof(_foc_ref));
}

FOC::~FOC(void)
{

}

bool FOC::init(void)
{
	_params_sub     = ipc_subscibe(IPC_ID(parameter_update));
	// _encoder_sub    = ipc_subscibe(IPC_ID(encoder));
	_foc_target_sub = ipc_subscibe(IPC_ID(foc_target));
	_foc_status_pub = ipc_active(IPC_ID(foc_status), &_foc_m);
	_led_pub        = ipc_active(IPC_ID(actuator_notify), &_led_state);

	_param_handles.foc_sample_v0_v7_handle = param_find("SAM_MODE");
	_param_handles.duty_max_handle         = param_find("DUTY_MAX");

	_param_handles.curr_d_p_handle         = param_find("CURR_D_P");
	_param_handles.curr_d_i_handle         = param_find("CURR_D_I");
	_param_handles.curr_q_p_handle         = param_find("CURR_Q_P");
	_param_handles.curr_q_i_handle         = param_find("CURR_Q_I");

	_param_handles.sensor_type_handle      = param_find("SENSOR_TYPE");
	_param_handles.decoupling_type_handle  = param_find("DECOUPLING");
	_param_handles.motor_r_handle          = param_find("MOTOR_R");
	_param_handles.motor_l_handle          = param_find("MOTOR_L");
	_param_handles.flux_linkage_handle     = param_find("FLUX_LINKAGE");

    MC_FOC::gHfi.init();
    MC_FOC::gObser.init();

	foc_adc_int  = perf_alloc(PC_INTERVAL, "adc_int");
	foc_task_int = perf_alloc(PC_INTERVAL, "foc_int");

	foc_adc_ela  = perf_alloc(PC_ELAPSED, "adc_ela");
	foc_task_ela = perf_alloc(PC_ELAPSED, "foc_ela");

	HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, ADC1_ConvCpltCallback);
	HAL_TIM_RegisterCallback(&htim2, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM2_PeriodElapsedCallback);

	// enc
	HAL_TIM_Base_Start_IT(&htim2);

	// motor
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

	// adc (be trigged)
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_current, 3);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_voltage, 3);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t *)adc_hall, 3);
    HAL_ADC_Start_DMA(&hadc4, (uint32_t *)adc_msic, 3);

	pwm_output_on();

	_handle = osThreadNew((osThreadFunc_t)foc_func, this, &foc_attributes);

    if (_handle != nullptr) {
        return true;
    }

	return false;
}

void FOC::current_calibration(void)
{
	volatile float phase_temp[3];
    
    pwm_output_off();
    
    memset(_mc_cfg.offset, 0, sizeof(_mc_cfg.offset));
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOS);
    __HAL_ADC_CLEAR_FLAG(&hadc4, ADC_FLAG_EOS);
    
    phase_temp[0] = 0.0f;
    phase_temp[1] = 0.0f;
    phase_temp[2] = 0.0f;
    
    for(uint16_t bIndex=0; bIndex <600; bIndex++) {
        while(!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOS) && !__HAL_ADC_GET_FLAG(&hadc4, ADC_FLAG_EOS)) { }

		if(adc_msic[2] == 0) {
			osDelay(1);
			continue;
		}
        
        phase_temp[0] += VREFINT * ((adc_current[0]*(VREF/4096))/(adc_msic[2]*(VREF/4096)));
        phase_temp[1] += VREFINT * ((adc_current[1]*(VREF/4096))/(adc_msic[2]*(VREF/4096)));
        phase_temp[2] += VREFINT * ((adc_current[2]*(VREF/4096))/(adc_msic[2]*(VREF/4096)));
        
        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOS);
        __HAL_ADC_CLEAR_FLAG(&hadc4, ADC_FLAG_EOS);
    }
    
    _mc_cfg.offset[0] = (phase_temp[0]/600.0f);
    _mc_cfg.offset[1] = (phase_temp[1]/600.0f);
    _mc_cfg.offset[2] = (phase_temp[2]/600.0f);
    
    pwm_output_on();

	Info_Debug("Current calibration success\n");
	Info_Debug("Ia-%d.%dV Ib-%d.%dV Ic-%d.%dV Ref-%d\n", (int)_mc_cfg.offset[0], (int)(_mc_cfg.offset[0]*1000)%1000, 
               (int)_mc_cfg.offset[1], (int)(_mc_cfg.offset[1]*1000)%1000,
               (int)_mc_cfg.offset[2], (int)(_mc_cfg.offset[2]*1000)%1000, adc_msic[2]);
}

void FOC::parameter_update(bool force)
{
	bool updated = force;
    if (!updated) {
		ipc_check(_params_sub, &updated);
	}
    if (updated) {
		parameter_update_s param_update;
		ipc_pull(IPC_ID(parameter_update), _params_sub, &param_update);
        
		param_get(_param_handles.foc_sample_v0_v7_handle, &_mc_cfg.foc_sample_v0_v7);
		param_get(_param_handles.duty_max_handle,         &_mc_cfg.duty_max);

		param_get(_param_handles.curr_d_p_handle,         &_mc_cfg.curr_d_p);
		param_get(_param_handles.curr_d_i_handle,         &_mc_cfg.curr_d_i);
		param_get(_param_handles.curr_q_p_handle,         &_mc_cfg.curr_q_p);
		param_get(_param_handles.curr_q_i_handle,         &_mc_cfg.curr_q_i);

		param_get(_param_handles.sensor_type_handle,      &_mc_cfg.sensor_type);
		param_get(_param_handles.decoupling_type_handle,  &_mc_cfg.decoupling_type);
		param_get(_param_handles.motor_r_handle,          &_mc_cfg.motor_r);
		param_get(_param_handles.motor_l_handle,          &_mc_cfg.motor_l);
		param_get(_param_handles.flux_linkage_handle,     &_mc_cfg.flux_linkage);

		_id_ctrl.kP(_mc_cfg.curr_d_p);
		_id_ctrl.kI(_mc_cfg.curr_d_i);
		_iq_ctrl.kP(_mc_cfg.curr_q_p);
		_iq_ctrl.kI(_mc_cfg.curr_q_i);
    }
}

void FOC::run(void *parammeter)
{
	parameter_update(true);

	while(1) {

		perf_begin(foc_task_ela);

		perf_count(foc_task_int);
		const uint64_t ts = micros();

		parameter_update(false);

		_refint = adc_msic[2];

		_foc_m.timestamp = ts;

		float adc_value = VREFINT * ((adc_msic[0]*(VREF/4096))/(_refint*(VREF/4096)));
		_foc_m.vbus = adc_value * ((RESISTANCE1+RESISTANCE2) / RESISTANCE2);
		_foc_m.temperature = 0;
		_foc_m.ctrl_mode = _foc_ref.ctrl_mode;

		float intd_limited = (2.0f / 3.0f) * _mc_cfg.duty_max * SQRT3_BY_2 * _foc_m.vbus;
		float indd = _id_ctrl.get_integrator();

		float indq_limited;
        arm_sqrt_f32((intd_limited*intd_limited) - (indd*indd), &indq_limited);

		_id_ctrl.imax(intd_limited);
		_iq_ctrl.imax(indq_limited);

		// push foc status
		ipc_push(IPC_ID(foc_status), _foc_status_pub, &_foc_m);

		if(_pre_foc_mode != _foc_ref.ctrl_mode) {
			_pre_foc_mode = _foc_ref.ctrl_mode;
			if(_foc_ref.ctrl_mode & MC_CTRL_ENABLE) {
				_led_state.led_status = LED_PATTERN_BGC_ARMED;
			} else {
				_led_state.led_status = LED_PATTERN_BGC_DISARM;
			}
			_led_state.timestamp = ts;
			ipc_push(IPC_ID(actuator_notify), _led_pub, &_led_state);
		}

		perf_end(foc_task_ela);

		osDelay(2);
	}
}

void FOC::foc_process(void)
{
	bool is_v7 = !(TIM1->CR1 & TIM_CR1_DIR);
    
    if (!_mc_cfg.foc_sample_v0_v7 && is_v7) {
        return;
    }

	float s, c;
	float error;
	float integrator;
	uint32_t top;

    float adc_value;

	const uint64_t ts = micros();

	const float dt = perf_count_isr(foc_adc_int);

	// HAL_GPIO_TogglePin(GPIO_TEST_1_GPIO_Port, GPIO_TEST_1_Pin);

	bool updated = false;
	ipc_check_isr(_foc_target_sub, &updated);
    if(updated) {
        ipc_pull_isr(IPC_ID(foc_target), _foc_target_sub, &_foc_ref);
    }
	
	// wait until foc task ready
	if (!(_foc_ref.ctrl_mode & MC_CTRL_ENABLE)) {
		_id_ctrl.reset_I();
		_iq_ctrl.reset_I();
		if(_power_state) {
			pwm_output_off();
			_power_state = false;
		}
		return;
	}

    perf_begin_isr(foc_adc_ela);

	_id_ctrl.set_dt(dt);
	_iq_ctrl.set_dt(dt);

	// get current date
	for (uint16_t i = 0; i < 3; i++)
	{
        adc_value = VREFINT * ((adc_current[i]*(VREF/4096))/(_refint*(VREF/4096)));
		_foc_m.i_phase[i] = (adc_value - _mc_cfg.offset[i]) / (ADC_CURRENT_AMP*ADC_CURRENT_OHM);
	}
	// get voltage date
	for (uint16_t i = 0; i < 3; i++)
	{
        adc_value = VREFINT * ((adc_voltage[i]*(VREF/4096))/(_refint*(VREF/4096)));
		_foc_m.v_phase[i] = adc_value * ((RESISTANCE1+RESISTANCE2) / RESISTANCE2);
	}

    if (_mc_cfg.foc_sample_v0_v7 && is_v7) {
        if (TIM1->CCR1 < TIM1->CCR2 && TIM1->CCR1 < TIM1->CCR3) {
            _foc_m.i_phase[0] = -(_foc_m.i_phase[1] + _foc_m.i_phase[2]);
        } else if (TIM1->CCR2 < TIM1->CCR1 && TIM1->CCR2 < TIM1->CCR3) {
            _foc_m.i_phase[1] = -(_foc_m.i_phase[0] + _foc_m.i_phase[2]);
        } else if (TIM1->CCR3 < TIM1->CCR1 && TIM1->CCR3 < TIM1->CCR2) {
            _foc_m.i_phase[2] = -(_foc_m.i_phase[0] + _foc_m.i_phase[1]);
        }
    } else {
        if (TIM1->CCR1 > TIM1->CCR2 && TIM1->CCR1 > TIM1->CCR3) {
            _foc_m.i_phase[0] = -(_foc_m.i_phase[1] + _foc_m.i_phase[2]);
        } else if (TIM1->CCR2 > TIM1->CCR1 && TIM1->CCR2 > TIM1->CCR3) {
            _foc_m.i_phase[1] = -(_foc_m.i_phase[0] + _foc_m.i_phase[2]);
        } else if (TIM1->CCR3 > TIM1->CCR1 && TIM1->CCR3 > TIM1->CCR2) {
            _foc_m.i_phase[2] = -(_foc_m.i_phase[0] + _foc_m.i_phase[1]);
        }
    }

	

	switch(_mc_cfg.sensor_type) {
	case MC_SENSOR_ENC:
	case MC_SENSOR_HAL:
		_encoder_data = MC_FOC::gEnc.get_encoder();
		_foc_m.phase_rad = _encoder_data.angle_e;
		break;
	case MC_SENSOR_HFI:
		break;
	case MC_SENSORLESS:
	default:
		break;
	}

	// electric period
	if(_foc_ref.ctrl_mode & MC_CTRL_OVERRIDE) {
		_foc_m.phase_rad = wrap_2PI(_foc_ref.phase_override);
	}

	s = arm_sin_f32(_foc_m.phase_rad);
	c = arm_cos_f32(_foc_m.phase_rad);

	// Clarke transform assuming balanced currents
	_foc_m.i_alpha = _foc_m.i_phase[0];
	_foc_m.i_beta = ONE_BY_SQRT3 * _foc_m.i_phase[0] + TWO_BY_SQRT3 * _foc_m.i_phase[1];

	// park transform
	_foc_m.i_d = c * _foc_m.i_alpha + s * _foc_m.i_beta;
	_foc_m.i_q = c * _foc_m.i_beta - s * _foc_m.i_alpha;

	// current loop
	if(_foc_ref.ctrl_mode & MC_CTRL_CURRENT) {
		error = _foc_ref.id_target - _foc_m.i_d;
		_id_ctrl.set_input_filter_d(error);
		integrator = _id_ctrl.get_integrator();
		if ((is_positive(integrator) && is_negative(error)) || (is_negative(integrator) && is_positive(error))) {
			integrator = _id_ctrl.get_i();
		}
		_foc_m.v_d = _id_ctrl.get_p() + integrator + _id_ctrl.get_ff(_foc_ref.id_target);

		error = _foc_ref.iq_target - _foc_m.i_q;
		_iq_ctrl.set_input_filter_d(error);
		integrator = _iq_ctrl.get_integrator();
		if ((is_positive(integrator) && is_negative(error)) || (is_negative(integrator) && is_positive(error))) {
			integrator = _iq_ctrl.get_i();
		}
		_foc_m.v_q = _iq_ctrl.get_p() + integrator + _iq_ctrl.get_ff(_foc_ref.iq_target);
	} else {
		_foc_m.v_d = _foc_ref.vd_target;
		_foc_m.v_q = _foc_ref.vq_target;
	}
	
	// Saturation (inscribed cycle)
	vector_2d_saturate(&_foc_m.v_d, &_foc_m.v_q, 
		(2.0f / 3.0f) * _mc_cfg.duty_max * SQRT3_BY_2 * _foc_m.vbus);

	_foc_m.mod_d = _foc_m.v_d / ((2.0 / 3.0) * _foc_m.vbus);
	_foc_m.mod_q = _foc_m.v_q / ((2.0 / 3.0) * _foc_m.vbus);

	_foc_m.duty = SIGN(_foc_m.v_q) * sqrtf(SQ(_foc_m.mod_d) + SQ(_foc_m.mod_q)) / SQRT3_BY_2;

	// re-park
	float mod_alpha = c * _foc_m.mod_d - s * _foc_m.mod_q;
	float mod_beta  = c * _foc_m.mod_q + s * _foc_m.mod_d;

	// svpwm
	top = TIM1->ARR;
	svm(-mod_alpha, -mod_beta, top, &_foc_m.pwm[0], &_foc_m.pwm[1], &_foc_m.pwm[2], &_foc_m.svm_sector);
	
	hal_pwm_duty_write(_foc_m.pwm[0], _foc_m.pwm[1], _foc_m.pwm[2]);
	
	if(!_power_state) {
		pwm_output_on();
		_power_state = true;
	}

	perf_end_isr(foc_adc_ela);
}

// Magnitude must not be larger than sqrt(3)/2, or 0.866
void FOC::svm(float alpha, float beta, uint32_t PWMHalfPeriod,
		uint32_t* tAout, uint32_t* tBout, uint32_t* tCout, uint32_t *svm_sector)
{
	uint32_t sector;

	if (beta >= 0.0f) {
		if (alpha >= 0.0f) {
			//quadrant I
			if (ONE_BY_SQRT3 * beta > alpha) {
				sector = 2;
			} else {
				sector = 1;
			}
		} else {
			//quadrant II
			if (-ONE_BY_SQRT3 * beta > alpha) {
				sector = 3;
			} else {
				sector = 2;
			}
		}
	} else {
		if (alpha >= 0.0f) {
			//quadrant IV5
			if (-ONE_BY_SQRT3 * beta > alpha) {
				sector = 5;
			} else {
				sector = 6;
			}
		} else {
			//quadrant III
			if (ONE_BY_SQRT3 * beta > alpha) {
				sector = 4;
			} else {
				sector = 5;
			}
		}
	}

	// PWM timings
	uint32_t tA, tB, tC;

	switch (sector) {

	// sector 1-2
	case 1: {
		// Vector on-times
		uint32_t t1 = (uint32_t)((alpha - ONE_BY_SQRT3 * beta) * PWMHalfPeriod);
		uint32_t t2 = (uint32_t)((TWO_BY_SQRT3 * beta) * PWMHalfPeriod);

		// PWM timings
		tA = (PWMHalfPeriod - t1 - t2) / 2;
		tB = tA + t1;
		tC = tB + t2;

		break;
	}

	// sector 2-3
	case 2: {
		// Vector on-times
		uint32_t t2 = (uint32_t)((alpha + ONE_BY_SQRT3 * beta) * PWMHalfPeriod);
		uint32_t t3 = (uint32_t)((-alpha + ONE_BY_SQRT3 * beta) * PWMHalfPeriod);

		// PWM timings
		tB = (PWMHalfPeriod - t2 - t3) / 2;
		tA = tB + t3;
		tC = tA + t2;

		break;
	}

	// sector 3-4
	case 3: {
		// Vector on-times
		uint32_t t3 = (uint32_t)((TWO_BY_SQRT3 * beta) * PWMHalfPeriod);
		uint32_t t4 = (uint32_t)((-alpha - ONE_BY_SQRT3 * beta) * PWMHalfPeriod);

		// PWM timings
		tB = (PWMHalfPeriod - t3 - t4) / 2;
		tC = tB + t3;
		tA = tC + t4;

		break;
	}

	// sector 4-5
	case 4: {
		// Vector on-times
		uint32_t t4 = (uint32_t)((-alpha + ONE_BY_SQRT3 * beta) * PWMHalfPeriod);
		uint32_t t5 = (uint32_t)((-TWO_BY_SQRT3 * beta) * PWMHalfPeriod);

		// PWM timings
		tC = (PWMHalfPeriod - t4 - t5) / 2;
		tB = tC + t5;
		tA = tB + t4;

		break;
	}

	// sector 5-6
	case 5: {
		// Vector on-times
		uint32_t t5 = (uint32_t)((-alpha - ONE_BY_SQRT3 * beta) * PWMHalfPeriod);
		uint32_t t6 = (uint32_t)((alpha - ONE_BY_SQRT3 * beta) * PWMHalfPeriod);

		// PWM timings
		tC = (PWMHalfPeriod - t5 - t6) / 2;
		tA = tC + t5;
		tB = tA + t6;

		break;
	}

	// sector 6-1
	case 6: {
		// Vector on-times
		uint32_t t6 = (uint32_t)((-TWO_BY_SQRT3 * beta) * PWMHalfPeriod);
		uint32_t t1 = (uint32_t)((alpha + ONE_BY_SQRT3 * beta) * PWMHalfPeriod);

		// PWM timings
		tA = (PWMHalfPeriod - t6 - t1) / 2;
		tC = tA + t1;
		tB = tC + t6;

		break;
	}
	}

	*tAout = tA;
	*tBout = tB;
	*tCout = tC;
	*svm_sector = sector;
}

int foc_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {
            MC_FOC::gFOC.init();
        }
        
        if (!strcmp(argv[i], "cali")) {
            MC_FOC::gFOC.current_calibration();
        }
    }
    return 1;
}

int enc_main(int argc, char *argv[])
{
    if (argc < 1) {
		Info_Debug("input argv error\n");
		return 1;
	}
    
    for(int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "start")) {
            MC_FOC::gEnc.init();
        }
    }
    return 1;
}

