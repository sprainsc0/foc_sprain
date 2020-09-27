#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"
#include "foc.h"

#include "topics/parameter_update.h"
#include "topics/foc_status.h"
#include "topics/obs_status.h"

class Observer
{
public:
    Observer(void);
    ~Observer(void);

    bool init(void);

    void parameter_update(bool force);

    void observer_update(float v_alpha, float v_beta, float i_alpha, float i_beta,
					 float dt, volatile float *phase, volatile foc_status_s *motor);

    void observer_idle(float *phase);

    void observer_task(volatile foc_status_s *motor);

private:
    struct obs_config {
        int   observer_type;
        float motor_r;
        float motor_l;
        float flux_linkage;

        float l_current_max;
        float foc_sat_comp;
        bool  foc_temp_comp;
        float foc_temp_comp_base_temp;

        float observer_gain;
        float observer_gain_slow;
    } _obs_cfg;

    struct {
        param_t observer_type_handle;
        param_t motor_r_handle;
        param_t motor_l_handle;
        param_t flux_linkage_handle;

        param_t l_current_max_handle;
        param_t foc_sat_comp_handle;
        param_t foc_temp_comp_handle;
        param_t foc_temp_comp_base_temp_handle;

        param_t observer_gain_handle;
        param_t observer_gain_slow_handle;
    } _param_handles;

    struct obs_status_s _obs_m;

    int _params_sub;
};

#endif
