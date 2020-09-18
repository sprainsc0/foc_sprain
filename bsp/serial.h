#ifndef UART_EX_H
#define UART_EX_H

#include "datatypes.h"
#include "platform.h"

// all these var is private
typedef struct
{
	bool ready;
	
	UART_HandleTypeDef *huart;

	osMessageQueueId_t tx_queue;
	osMessageQueueId_t rx_queue;
	uint32_t queue_len;

	uint8_t *tx_buffer;
	uint8_t *rx_buffer;
	uint32_t buffer_sz;

	// tx r/w is straight line
	uint32_t rx_write_cnt;
	uint32_t rx_read_cnt;

} uart_t;

void hal_uart_open(uart_t *h);
bool hal_uart_write(uart_t *h, void *data, uint32_t size, uint32_t time);
uint32_t hal_uart_read(uart_t *h, void *data, uint32_t time);
void hal_uart_write_handle(uart_t *h);
void hal_uart_read_handle(uart_t *h);

#endif

