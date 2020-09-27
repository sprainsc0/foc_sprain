#ifndef _BEAST_PARAM_H
#define _BEAST_PARAM_H

#include <stdint.h>
#include "param.h"

#ifdef __cplusplus
extern "C"{
#endif
    
struct bgc_parameters_t {
    /* -------------------acc calibration parameter------------------- */
    const struct param_info_s __param__SAM_MODE;
    const struct param_info_s __param__DUTY_MAX;
    
    const struct param_info_s __param__ENC_TYPE;
    const struct param_info_s __param__PAIR_NUM;
    const struct param_info_s __param__ENC_E_OFFSET;
    const struct param_info_s __param__ENC_M_OFFSET;
    const struct param_info_s __param__ENC_E_INVERT;
    const struct param_info_s __param__ENC_M_INVERT;

    const struct param_info_s __param__CURR_D_P;
    const struct param_info_s __param__CURR_D_I;
    const struct param_info_s __param__CURR_Q_P;
    const struct param_info_s __param__CURR_Q_I;

    const struct param_info_s __param__SENSOR_TYPE;
    const struct param_info_s __param__DECOUPLING;
    const struct param_info_s __param__MOTOR_R;
    const struct param_info_s __param__MOTOR_L;
    const struct param_info_s __param__MOTOR_L_DIFF;
    const struct param_info_s __param__FLUX_LINKAGE;

    const struct param_info_s __param__OBSERVER_TYPE;
    const struct param_info_s __param__CURRENT_MAX;
    const struct param_info_s __param__SAT_COMP;  
    const struct param_info_s __param__TEMP_COMP;  
    const struct param_info_s __param__TEMP_BASE;   
    const struct param_info_s __param__OBS_GAIN;    
    const struct param_info_s __param__OBS_GAIN_SLOW;

	const unsigned int param_count;
};

extern const struct bgc_parameters_t foc_parameters;

#ifdef __cplusplus
}
#endif
#endif