#include "foc_parameter.h"

const struct bgc_parameters_t foc_parameters = {
    //    NAME             TYPE              VOLA            DEFAULT
    /* -------------------acc calibration parameter 12------------------- */
	{"CTRL_LOOP",    PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 1 },
    {"SAM_MODE",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 1 },
    {"DUTY_MAX",     PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.8f },
    
    {"ENC_TYPE",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"PAIR_NUM",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 8 },
    {"ENC_E_OFFSET", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },
    {"ENC_M_OFFSET", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },
    {"ENC_E_INVERT", PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"ENC_M_INVERT", PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },

    // parameter count
    9
};