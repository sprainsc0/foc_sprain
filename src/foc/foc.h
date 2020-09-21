#ifndef MC_FOC_H
#define MC_FOC_H

#include <datatypes.h>
#include <stdint.h>
#include "cmsis_os.h"
#include <string.h>
#include "uPerf.h"
#include <pid/pid.h>
#include <ipc.h>
#include "param.h"

#include "topics/parameter_update.h"
#include "topics/actuator_notify.h"
#include "topics/encoder.h"
#include "topics/foc_status.h"
#include "topics/foc_target.h"

#define ADC_CURRENT_OHM		                  0.1f
#define ADC_CURRENT_AMP		                  20.0f
#define VREFINT                               1.2f
#define VREF                                  3.3f
            
#define RESISTANCE1                           30.0f  // 30K
#define RESISTANCE2                           3.3f   // 3.3K

#define CURRENT_RATE_DT                       0.0001f

class FOC
{
public:
    FOC(void);
    ~FOC(void);

    void *_param;
    void run(void *parameter);

    bool init(void);
    void foc_process(void);

    void parameter_update(bool force);

    void current_calibration(void);

protected:
    osThreadId_t _handle;

private:
    static bool _power_state;
    uint8_t _pre_foc_mode;
    
    struct motor_config {
        int   foc_sample_v0_v7;
        float offset[3];
        float duty_max;

        float curr_d_p;
        float curr_d_i;
        float curr_q_p;
        float curr_q_i;
    } _mc_cfg;

    struct {
        param_t foc_sample_v0_v7_handle;
        param_t duty_max_handle;

        param_t curr_d_p_handle;
        param_t curr_d_i_handle;
        param_t curr_q_p_handle;
        param_t curr_q_i_handle;
    } _param_handles;

    // 参考电压
    uint16_t _refint;

    int _params_sub;

    // int _encoder_sub;
    struct encoder_s _encoder_data;

    int _foc_target_sub;
    struct foc_target_s _foc_ref;

    orb_advert_t _foc_status_pub;
    struct foc_status_s _foc_m;

    orb_advert_t _led_pub;
    struct actuator_notify_s _led_state;

    PID _id_ctrl;
    PID _iq_ctrl;

    void svm(float alpha, float beta, uint32_t PWMHalfPeriod,
		uint32_t* tAout, uint32_t* tBout, uint32_t* tCout, uint32_t *svm_sector);
    
    perf_counter_t foc_adc_int;
    perf_counter_t foc_adc_ela;

    perf_counter_t foc_task_int;
    perf_counter_t foc_task_ela;
};

#endif
