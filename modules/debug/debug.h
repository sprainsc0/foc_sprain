#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
extern  "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#define Info_Debug(...) SEGGER_RTT_printf(0, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
