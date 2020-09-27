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

#define SIGN(x)				((x < 0) ? -1 : 1)

#define SQ(x)				((x) * (x))

#define UTILS_LP_FAST(value, sample, filter_constant)	(value -= (filter_constant) * ((value) - (sample)))

// nan and infinity check for floats
#define UTILS_IS_INF(x)		((x) == (1.0 / 0.0) || (x) == (-1.0 / 0.0))
#define UTILS_IS_NAN(x)		((x) != (x))
#define UTILS_NAN_ZERO(x)	(x = UTILS_IS_NAN(x) ? 0.0 : x)

bool is_zero(const float fVal1);
bool is_positive(const float fVal1);
bool is_negative(const float fVal1);

float wrap_360(float angle);
float wrap_180(float angle);
float wrap_2PI(float radian);
float wrap_PI(float radian);

float angles_mean(float *angles, int angles_num);
float angles_mean2(float *angles, int angles_num);
float angle_difference(float angle1, float angle2);
float angles_variance(float *angles, int angles_num);
int utils_truncate_number_abs(float *number, float max);
bool utils_saturate_vector_2d(float *x, float *y, float max);

float utils_fast_atan2(float y, float x);

int utils_map_int(int x, int in_min, int in_max, int out_min, int out_max);
float utils_map(float x, float in_min, float in_max, float out_min, float out_max);

void utils_fft32_bin0(float *real_in, float *real, float *imag);
void utils_fft32_bin1(float *real_in, float *real, float *imag);
void utils_fft32_bin2(float *real_in, float *real, float *imag);
void utils_fft16_bin0(float *real_in, float *real, float *imag);
void utils_fft16_bin1(float *real_in, float *real, float *imag);
void utils_fft16_bin2(float *real_in, float *real, float *imag);
void utils_fft8_bin0(float *real_in, float *real, float *imag);
void utils_fft8_bin1(float *real_in, float *real, float *imag);
void utils_fft8_bin2(float *real_in, float *real, float *imag);

extern const float utils_tab_sin_32_1[];
extern const float utils_tab_sin_32_2[];
extern const float utils_tab_cos_32_1[];
extern const float utils_tab_cos_32_2[];

#ifdef __cplusplus
}
#endif
#endif
