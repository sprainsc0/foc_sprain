#include "platform.h"
#include "as5048.h"


// Max output sample rate for AS5048: 11.25Khz.
// Samplerate: 10Khz, FOC calc rate: 20Khz.
// So use a simple interpolation to increase the points(2 point per INT).

#define AS5048_REG_ANGLE       0x3FFF

as5048_t as5048;

static void en(bool en)
{
	if (en)
		HAL_GPIO_WritePin(as5048.en_port, as5048.en_pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(as5048.en_port, as5048.en_pin, GPIO_PIN_SET);
}

void hal_as5048_init(void)
{
	as5048.ready = false;
	as5048.hspi = &hspi1;
	as5048.en_port = SPI1_CS_1_GPIO_Port;
	as5048.en_pin = SPI1_CS_1_Pin;
	
	en(false);

	as5048.ready = true;
}

bool hal_as5048_read(uint32_t *raw)
{
	if (!as5048.ready)
		return false;

	static uint16_t value_tx, value_rx;
	
	value_tx = AS5048_REG_ANGLE;
	
	en(true);
	HAL_SPI_TransmitReceive(as5048.hspi, (uint8_t *)&value_tx, (uint8_t *)&value_rx, 1, 1);
	en(false);

	*raw = value_rx & 0x3FFF;
	return true;
}



