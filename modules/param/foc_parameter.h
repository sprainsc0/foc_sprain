#ifndef _BEAST_PARAM_H
#define _BEAST_PARAM_H

#include <stdint.h>
#include "param.h"

#ifdef __cplusplus
extern "C"{
#endif
    
struct bgc_parameters_t {
    /* -------------------acc calibration parameter------------------- */
	const struct param_info_s __param__CTRL_LOOP;
    const struct param_info_s __param__SAM_MODE;
    const struct param_info_s __param__DUTY_MAX;
    
    const struct param_info_s __param__ENC_TYPE;
    const struct param_info_s __param__PAIR_NUM;
    const struct param_info_s __param__ENC_E_OFFSET;
    const struct param_info_s __param__ENC_M_OFFSET;
    const struct param_info_s __param__ENC_E_INVERT;
    const struct param_info_s __param__ENC_M_INVERT;

	const unsigned int param_count;
};

extern const struct bgc_parameters_t foc_parameters;

#ifdef __cplusplus
}
#endif
#endif