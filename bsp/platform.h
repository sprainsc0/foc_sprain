#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __cplusplus
extern "C"{
#endif

#include "main.h"
#include "cmsis_os.h"

#include "tim.h"
#include "adc.h"
#include "gpio.h"

void platform_init(void);
void hal_pwm_duty_write(uint32_t duty_a, uint32_t duty_b, uint32_t duty_c);
void hal_update_samp(uint32_t samp);

void pwm_output_on(void);
void pwm_output_off(void);

uint8_t calculate_deadtime(float deadtime_ns, float core_clock_freq);

#ifdef __cplusplus
}
#endif

#endif
