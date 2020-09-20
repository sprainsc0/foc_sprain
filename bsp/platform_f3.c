#include "platform.h"
#include "serial.h"
#include "uavcan.h"

void platform_init(void)
{
    HAL_TIM_Base_Start(&htim3);

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
	// __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty_a);
	// __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty_b);
	// __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty_c);
}

void pwm_output_on(void)
{
	HAL_GPIO_WritePin(MOS_EN_A_GPIO_Port, MOS_EN_A_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MOS_EN_B_GPIO_Port, MOS_EN_B_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MOS_EN_C_GPIO_Port, MOS_EN_C_Pin, GPIO_PIN_SET);
}

void pwm_output_off(void)
{
	HAL_GPIO_WritePin(MOS_EN_A_GPIO_Port, MOS_EN_A_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOS_EN_B_GPIO_Port, MOS_EN_B_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(MOS_EN_C_GPIO_Port, MOS_EN_C_Pin, GPIO_PIN_RESET);
}
