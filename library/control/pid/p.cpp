#include <B_Math.h>
#include "p.h"


float P::get_p(float error) const
{
    return (float)error * _kp;
}
