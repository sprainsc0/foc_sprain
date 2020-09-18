#ifndef AS_5048_H
#define AS_5048_H

#include "platform.h"
#include "datatypes.h"

// if you wanna mount multi ma730 dev, have to write in this way:
typedef struct
{
	bool ready;
	
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *en_port;
	uint16_t en_pin;
} as5048_t;


void hal_as5048_init(void);
bool hal_as5048_read(uint32_t *raw);

#endif

