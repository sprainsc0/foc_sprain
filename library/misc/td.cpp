#include "utils.h"
#include "td.h"


float TD::fhan(float x1, float x2, float r0, float h0)
{
    float d = h0 * h0 * r0;
	float a0 = h0 * x2;
	float y = x1 + a0;
	float a1 = sqrtf(d*(d + 8.0f*fabsf(y)));
	float a2 = a0 + SIGN(y)*(a1-d)*0.5f;
	float sy = (SIGN(y+d) - SIGN(y-d))*0.5f;
	float a = (a0 + y - a2)*sy + a2;
	float sa = (SIGN(a+d) - SIGN(a-d))*0.5f;
	
	return -r0*(a/d - SIGN(a))*sa - r0*SIGN(a);
}

void TD::reset(void)
{
    _x1 = _x2 = 0.0f;
}

void TD::run(float value)
{
	float err = _x1 - value;

    float fh = fhan(err, _x2, _r0, _h0);
	
	_x1 += _h * _x2;
	_x2 += _h * fh;
}