#ifndef DATATYPES_H
#define DATATYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <math.h>				// to slow
#include <stm32f3xx.h>
#include <float.h>
#include <arm_math.h>

#define ONE_BY_SQRT3			(0.57735026919f)
#define TWO_BY_SQRT3			(2.0f * 0.57735026919f)
#define SQRT3_BY_2				(0.86602540378f)

#ifndef _MATH_DEFINES_DEFINED
    #define _MATH_DEFINES_DEFINED
    // Definitions of useful mathematical constants
    //
    // Define _USE_MATH_DEFINES before including <math.h> to expose these macro
    // definitions for common math constants.  These are placed under an #ifdef
    // since these commonly-defined names are not part of the C or C++ standards
    #define M_E        2.71828182845904523536f   // e
    #define M_LOG2E    1.44269504088896340736f   // log2(e)
    #define M_LOG10E   0.434294481903251827651f  // log10(e)
    #define M_LN2      0.693147180559945309417f  // ln(2)
    #define M_LN10     2.30258509299404568402f   // ln(10)
    #define M_PI       3.14159265358979323846f   // pi
    #define M_2PI      (M_PI * 2)
#ifndef M_PI_2
    #define M_PI_2     1.57079632679489661923f   // pi/2
#endif
    #define M_PI_4     0.785398163397448309616f  // pi/4
    #define M_1_PI     0.318309886183790671538f  // 1/pi
    #define M_2_PI     0.636619772367581343076f  // 2/pi
    #define M_2_SQRTPI 1.12837916709551257390f   // 2/sqrt(pi)
    #define M_SQRT2    1.41421356237309504880f   // sqrt(2)
    #define M_SQRT1_2  0.707106781186547524401f  // 1/sqrt(2)
#endif
#ifdef __cplusplus
}
#endif
#endif

