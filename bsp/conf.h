#ifndef HW_CONF_H
#define HW_CONF_H

#define HSECLK 						(16000000)

#ifdef D1_FOC
#define HARDWARE_VEWRSION			(0xD101)
#elif D2_FOC
#define HARDWARE_VEWRSION			(0xD201)
#endif


#define ADC_SAMPLE_POINT_FORWARD	100
#define ADC_SATURATE_BOUND			50

#define CURRENT_SHUNT_RES			20
#define CURRENT_AMP_GAIN			(7.15f+1)
#define FACTOR_CURRENT				(3300.0f * 1000 / 4095.0f / CURRENT_SHUNT_RES / CURRENT_AMP_GAIN)
#define SHUNT_NUM					(3)

// 云台实际给的范围跟测到的电流范围有关系
#define MC_FOC_IQ_MAX_FACT			(8000)
#define MC_FOC_VQ_MAX_FACT			(SQRT3_BY_2*1000)

#endif
