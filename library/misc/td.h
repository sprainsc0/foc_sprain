#ifndef __TD_H__
#define __TD_H__

#include <stdlib.h>
#include <cmath>

class TD {
public:

    TD(float h, float r0, float h0)
    {
        _h  = h;
        _r0 = r0;
        _h0 = h0;
        _x1 = _x2 = 0.0f;
    }

    void    run(float value);

    float    x1(void) const { return _x1; }
    float    x2(void) const { return _x2; }

    void     reset(void);

private:
    float _x1;
	float _x2;
	float _r0;
	float _h0;
	float _h;

    float fhan(float x1, float x2, float r0, float h0);
};

#endif