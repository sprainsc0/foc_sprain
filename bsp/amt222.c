#include "amt222.h"
#include <stdint.h>
#include "delay.h"

static void en(bool en)
{
	if (en)
		HAL_GPIO_WritePin(SPI1_CS_2_GPIO_Port, SPI1_CS_2_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(SPI1_CS_2_GPIO_Port, SPI1_CS_2_Pin, GPIO_PIN_SET);
}

bool hal_amt222_read(uint32_t *raw)
{
	uint8_t value_tx[2], value_rx[2];
	uint16_t angle;
	uint8_t k0, k1, even, odd;
    
    value_tx[0] = 0x00;
    value_tx[1] = 0x00;
    
    __disable_irq();
	en(true);
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&value_tx[0], (uint8_t *)&value_rx[0], 1, 1);
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&value_tx[1], (uint8_t *)&value_rx[1], 1, 1);
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	en(false);
    __enable_irq();

	angle = (value_rx[0] & 0x3F) << 8;
	angle |= (value_rx[1] & 0xFF);

	*raw = angle;

	k0 = k1 = 0;
	for (size_t i = 0; i < 14 / 2; i++)
	{
		k0 ^= (angle & 0x01);
		angle >>= 1;

		k1 ^= (angle & 0x01);
		angle >>= 1;
	}

	k0 &= 0x01;
	k1 &= 0x01;
	k0 = !k0;
	k1 = !k1;
	
	even = (value_rx[0] >> 6) & 0x01;
	odd  = (value_rx[0] >> 7) & 0x01;

	if (k0 == even && k1 == odd) {
		return true;
	} else {
		return false;
	}
}

void hal_amt222_reset(void)
{
	uint8_t value_tx[2], value_rx[2];
    
    value_tx[0] = 0x00;
    value_tx[1] = 0x60;
    
    __disable_irq();
	en(true);
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&value_tx[0], (uint8_t *)&value_rx[0], 1, 1);
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
    hal_delay_us(2);
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&value_tx[1], (uint8_t *)&value_rx[1], 1, 1);
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	en(false);
    __enable_irq();
}