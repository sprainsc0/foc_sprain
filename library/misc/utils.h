#ifndef _UTILS_H
#define _UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "datatypes.h"

#define LPF_FAST(_value, _sample, _factor) \
									(_value -= (_factor) * ((_value) - (_sample)))
#define ELEMENT_OF(__BUFFER) 		(sizeof(__BUFFER) / sizeof(*(__BUFFER)))
#define OFFSET_ADDR(type, ele) 		((unsigned int)(&((type *)0)->ele))
#define DEG_TO_RAD(angleDegrees) 	((angleDegrees)*M_PI / 180.0f)
#define RAD_TO_DEG(angleRadians) 	((angleRadians)*180.0f / M_PI)

bool is_zero(const float fVal1);
bool is_positive(const float fVal1);
bool is_negative(const float fVal1);

float wrap_360(float angle);
float wrap_180(float angle);
float wrap_2PI(float radian);
float wrap_PI(float radian);

float angles_mean(float *angles, int angles_num);
float angles_mean2(float *angles, int angles_num);
float angles_variance(float *angles, int angles_num);
bool vector_2d_saturate(float *x, float *y, float max);

#ifdef __cplusplus
}
#endif
#endif
