#ifndef __HFI_H__
#define __HFI_H__

#include <stdint.h>
#include "cmsis_os.h"
#include "ipc.h"
#include "uPerf.h"
#include "param.h"

#include "topics/parameter_update.h"
#include "topics/foc_status.h"
#include "topics/hfi_status.h"

class HFI
{
public:
    HFI(void);

    void *_param;
    void run(void *parameter);

    bool init(void *param);

    void parameter_update(bool force);

    void hfi_sample(bool ready, float mod_alpha, float mod_beta, volatile foc_status_s *motor);

    void hfi_init(foc_hfi_samples samples);

    void hfi_update(void);

    void hfi_idle(void);

    float mod_alpha(void) const { return _mod_alpha_tmp; }
    float mod_beta(void) const { return _mod_beta_tmp; }

protected:
    osThreadId_t _handle;
private:

    struct hfi_config {
        int   foc_sample_v0_v7;
        float motor_r;
        float motor_l;
        float motor_l_diff;
        float flux_linkage;

        float l_current_max;

        float hfi_voltage_start;
        float hfi_voltage_run;
        float hfi_voltage_max;
        int   hfi_start_samples;
        float hfi_obs_ovr_sec;
        int   hfi_samples;
    } _hfi_cfg;

    struct {
        param_t foc_sample_v0_v7_handle;
        param_t motor_r_handle;
        param_t motor_l_handle;
        param_t motor_l_diff_handle;
        param_t flux_linkage_handle;

        param_t l_current_max_handle;

        param_t hfi_voltage_start_handle;
        param_t hfi_voltage_run_handle;
        param_t hfi_voltage_max_handle;
        param_t hfi_start_samples_handle;
        param_t hfi_obs_ovr_sec_handle;
        param_t hfi_samples_handle;
    } _param_handles;

    int _params_sub;

    struct hfi_status_s _hfi_m;

    float _mod_alpha_tmp;
    float _mod_beta_tmp;

    const uint32_t _foc_f_sw = 25000;
};

#endif
