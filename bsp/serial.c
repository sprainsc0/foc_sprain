#include "platform.h"
#include "uart_ex.h"

/*
	@note: Test code (HAL lib has some problem in tx rx in the same time!)
	if(hal_uart_read(&huart_debug, &msg, portMAX_DELAY))
		hal_uart_write(&huart_debug, &msg, portMAX_DELAY);
*/

typedef struct
{
	uint8_t load[512];
	uint32_t size;
} uart_msg_t;

void hal_uart_open(uart_t *h)
{
	uint32_t size;

	size = h->buffer_sz;

	h->tx_buffer = (uint8_t *)pvPortMalloc(size);
	memset(h->tx_buffer, 0, size);

	h->rx_buffer = (uint8_t *)pvPortMalloc(size);
	memset(h->rx_buffer, 0, size);
	h->rx_read_cnt = h->rx_write_cnt = 0;

	h->tx_queue = osMessageQueueNew(h->queue_len, size + 4, NULL);
	h->rx_queue = osMessageQueueNew(h->queue_len, size + 4, NULL);

	HAL_UART_Receive_DMA(h->huart, h->rx_buffer, size);
	
	h->ready = true;
}

void hal_uart_write_handle(uart_t *h)
{
	if(!h->ready)
		return;

	uart_msg_t msg;

	if (HAL_DMA_IDLE(h->huart))
	{
		if (osMessageQueueGet(h->tx_queue, &msg, NULL, 0) == osOK)
		{
			memcpy(h->tx_buffer, msg.load, msg.size);
			HAL_UART_Transmit_DMA(h->huart, h->tx_buffer, msg.size);
		}
	}
}

void hal_uart_read_handle(uart_t *h)
{
	if(!h->ready)
		return;

	uart_msg_t msg;
	uint32_t size;
	uint32_t dma_cnt;

	size = h->buffer_sz;
	dma_cnt = __HAL_DMA_GET_COUNTER(h->huart->hdmarx);
	h->rx_write_cnt = size - dma_cnt;
	msg.size = 0;
	
	while ( h->rx_read_cnt != h->rx_write_cnt)
	{
		msg.load[msg.size] = h->rx_buffer[h->rx_read_cnt];
		
		h->rx_read_cnt++;
		if (h->rx_read_cnt >= size)
			h->rx_read_cnt = 0;
		
		msg.size++;
		if (msg.size >= size)
		{
			break;
		}
	}
	
	if(msg.size)
		osMessageQueuePut(h->rx_queue, &msg, 0, 0);
}

// TODO: CAN'T send a msg longer than buffer size.
// (But if I did it, the message just been token apart, result uart signal
// a idle area, so its better to increase the buffer size)
bool hal_uart_write(uart_t *h, void *data, uint32_t size, uint32_t time)
{
	if(!h->ready)
		return false;
	
	if ((msg->size <= h->buffer_sz) && osMessageQueuePut(h->tx_queue, msg, NULL, time) == osOK)
		return true;
	else
		return false;
}

uint32_t hal_uart_read(uart_t *h, void *data, uint32_t time)
{
	if(!h->ready)
		return false;

	if (osMessageQueueGet(h->rx_queue, msg, NULL, time) == osOK)
		return true;
	else
		return false;
}
