#include "platform.h"
#include "serial.h"
#include "uavcan.h"

void platform_init(void)
{
    HAL_TIM_Base_Start(&htim5);
	TIM5->CNT = 0;

	hal_uart_init();

	pwm_output_off();
}

void hal_pwm_duty_write(uint32_t duty_a, uint32_t duty_b, uint32_t duty_c)
{
	TIM1->CR1 |= TIM_CR1_UDIS;
	TIM1->CCR1 = duty_a;
	TIM1->CCR2 = duty_b;
	TIM1->CCR3 = duty_c;
	TIM1->CR1 &= ~TIM_CR1_UDIS;
}

void hal_update_samp(uint32_t samp)
{
	TIM2->CCR2 = (samp / 2);
}

void pwm_output_on(void)
{
	HAL_GPIO_WritePin(GATE_EN_GPIO_Port, GATE_EN_Pin, GPIO_PIN_SET);
}

void pwm_output_off(void)
{
	HAL_GPIO_WritePin(GATE_EN_GPIO_Port, GATE_EN_Pin, GPIO_PIN_RESET);
}

uint8_t calculate_deadtime(float deadtime_ns, float core_clock_freq) 
{
	uint8_t DTG = 0;
	float timebase = 1.0f / (core_clock_freq / 1000000.0f) * 1000.0f;

	if (deadtime_ns <= (timebase * 127.0f)) {
		DTG = (uint8_t)(deadtime_ns / timebase);
	} else {
		if (deadtime_ns <= ((63.0f + 64.0f) * 2.0f * timebase)) {
			DTG = (uint8_t)(deadtime_ns / (2.0f * timebase) - 64.0f);
			DTG |= 0x80;
		} else {
			if (deadtime_ns <= ((31.0f + 32.0f) * 8.0f * timebase)) {
				DTG = (uint8_t)(deadtime_ns / (8.0f * timebase) - 32.0f);
				DTG |= 0xC0;
			} else {
				if (deadtime_ns <= ((31.0f + 32) * 16 * timebase)) {
					DTG = (uint8_t)(deadtime_ns / (16.0f * timebase) - 32.0f);
					DTG |= 0xE0;
				} else {
					// Deadtime requested is longer than max achievable. Set deadtime at
					// longest possible value
					DTG = 0xFF;
					assert_param(1); //catch this
				}
			}
		}
	}

	return DTG;
}
