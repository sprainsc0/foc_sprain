#include "foc_parameter.h"

const struct bgc_parameters_t foc_parameters = {
    //    NAME             TYPE              VOLA            DEFAULT
    /* -------------------acc calibration parameter 12------------------- */
    {"SAM_MODE",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
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

    {"SENSOR_TYPE",   PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"DECOUPLING",    PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"MOTOR_R",       PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.608f },
    {"MOTOR_L",       PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 4.63e-4f },
    {"MOTOR_L_DIFF",  PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },
    {"FLUX_LINKAGE",  PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },

    {"OBSERVER_TYPE", PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"CURRENT_MAX",   PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 10.0f },
    {"SAT_COMP",      PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.0f },
    {"TEMP_COMP",     PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 0 },
    {"TEMP_BASE",     PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 25.0f },
    {"OBS_GAIN",      PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 9e7 },
    {"OBS_GAIN_SLOW", PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.05f },

    {"HFI_START_SAM", PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 65 },
    {"HFI_V_START",   PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 8.0f },
    {"HFI_V_RUN",     PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 8.0f },
    {"HFI_V_MAX",     PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 8.0f },
    {"HFI_OBS_OVER",  PARAM_TYPE_FLOAT, .volatile_param = 0,.val.f = 0.001f },
    {"HFI_SAM_NUM",   PARAM_TYPE_INT32, .volatile_param = 0,.val.i = 1 },

    // parameter count
    30
};