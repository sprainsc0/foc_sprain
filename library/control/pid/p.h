#ifndef __P_CONTROL_H__
#define __P_CONTROL_H__

#include <stdlib.h>
#include <cmath>

class P {
public:

    P(const float &initial_p = 0.0f)
    {
        _kp = initial_p;
    }

    float       get_p(float error) const;

    void operator() (const float p) { _kp = p; }

    float    &kP() { return _kp; }
    const float &kP() const { return _kp; }
    void        kP(const float v) { _kp = v; }

private:
    float        _kp;
};

#endif