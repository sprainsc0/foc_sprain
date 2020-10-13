#ifndef  __BSP_HRT_TIMER_H__
#define  __BSP_HRT_TIMER_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f4xx_hal.h"

extern uint64_t micros(void);

extern uint64_t millis(void);

#ifdef __cplusplus
}
#endif
#endif
