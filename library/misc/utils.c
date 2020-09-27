#include "utils.h"


float wrap_360(float angle)
{
    float res = fmodf(angle, 360.f);
    if (res < 0) {
        res += 360.f;
    }
    return res;
}

float wrap_180(float angle)
{
	float res = wrap_360(angle);
    if (res > 180.f) {
        res -= 360.f;
    }
    return res;
}

float wrap_2PI(float radian)
{
	float res = fmodf(radian, M_2PI);
    if (res < 0) {
        res += M_2PI;
    }
    return res;
}

float wrap_PI(float radian)
{
	float res = wrap_2PI(radian);
    if (res > M_PI) {
        res -= M_2PI;
    }
    return res;
}

bool is_zero(const float fVal1) 
{
    return (fabsf(fVal1) < FLT_EPSILON);
}

bool is_positive(const float fVal1) 
{
    return (fVal1 >= FLT_EPSILON);
}

bool is_negative(const float fVal1) 
{
    return (fVal1 <= (-1.0 * FLT_EPSILON));
}

// intuitive way
float angles_mean(float *angles, int size)
{
	float angle_sum;
	float angle, angle0;
	float mean;

	angle0 = *angles++;
	angle_sum = 0;
	
	for (uint32_t i = 0; i < size; i++) {
		angle = *angles;
		
		if (angle - angle0 > M_PI)
			angle -= M_2PI;
		else if (angle0 - angle > M_PI)
			angle += M_2PI;
		
		angle_sum += angle;
		
		angles++;
	}
	
	mean = angle_sum / size;
	
	return wrap_2PI(mean);
}

// use sin cos
float angles_mean2(float *angles, int size)
{
	float mean;
	float x, y;
	float rad;

	x = 0;
	y = 0;

	for (uint32_t i = 0; i < size; i++)
	{
		rad = *angles;
		angles++;
		x += arm_cos_f32(rad);
		y += arm_sin_f32(rad);
	}

	mean = (atan2(y / size, x / size));
	return wrap_2PI(mean);
}

float angle_difference(float angle1, float angle2)
{
	float difference = angle1 - angle2;
	while (difference < -M_PI)
		difference += M_2PI;
	while (difference > M_PI)
		difference -= M_2PI;
	return difference;
}

float angles_variance(float *angles, int size)
{
	float mean, variance, diff;
	float x, y;
	float rad;

	mean = angles_mean2(angles, size);
	
	x = 0;
	y = 0;

	for (uint32_t i = 0; i < size; i++)
	{
		diff = angle_difference(*angles, mean);
		angles++;
		rad = diff;

		x += arm_cos_f32(rad * rad);
		y += arm_sin_f32(rad * rad);
	}
	variance = atan2f(y, x);
	
	return variance;
}

bool vector_2d_saturate(float *x, float *y, float max)
{
	bool retval = false;
	float mag = sqrtf(*x * *x + *y * *y);
	max = fabsf(max);

	if (mag < (float)(1e-10))
	{
		mag = 1e-10;
	}

	if (mag > max)
	{
		const float f = max / mag;
		*x *= f;
		*y *= f;
		retval = true;
	}

	return retval;
}

float utils_fast_atan2(float y, float x) 
{
	float abs_y = fabsf(y) + 1e-20; // kludge to prevent 0/0 condition
	float angle;

	if (x >= 0) {
		float r = (x - abs_y) / (x + abs_y);
		float rsq = r * r;
		angle = ((0.1963 * rsq) - 0.9817) * r + (M_PI / 4.0);
	} else {
		float r = (x + abs_y) / (abs_y - x);
		float rsq = r * r;
		angle = ((0.1963 * rsq) - 0.9817) * r + (3.0 * M_PI / 4.0);
	}

	if (y < 0) {
		return(-angle);
	} else {
		return(angle);
	}
}

int utils_truncate_number_abs(float *number, float max) 
{
	int did_trunc = 0;

	if (*number > max) {
		*number = max;
		did_trunc = 1;
	} else if (*number < -max) {
		*number = -max;
		did_trunc = 1;
	}

	return did_trunc;
}

float utils_map(float x, float in_min, float in_max, float out_min, float out_max) 
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int utils_map_int(int x, int in_min, int in_max, int out_min, int out_max) 
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

