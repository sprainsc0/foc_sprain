#include "foc_parameter.h"

const struct bgc_parameters_t foc_parameters = {
    //    NAME             TYPE              VOLA            DEFAULT
    /* -------------------acc calibration parameter 12------------------- */
    {"SAM_MODE",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 1 },
    {"DUTY_MAX",     PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.8f },
    
    {"ENC_TYPE",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"PAIR_NUM",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 8 },
    {"ENC_E_OFFSET", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },
    {"ENC_M_OFFSET", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },
    {"ENC_E_INVERT", PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"ENC_M_INVERT", PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },

    {"CURR_D_P", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 17.0f },
    {"CURR_D_I", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 4.0f },
    {"CURR_Q_P", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 17.0f },
    {"CURR_Q_I", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 4.0f },

    // parameter count
    12
};