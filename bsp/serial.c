#include "serial.h"
#include "ringbuffer.h"
#include <semphr.h>

static struct ringbuffer uart6_ring;
static SemaphoreHandle_t uart6_sem;
#define UART6RINGBUFFER_SIZE     1024
static uint8_t uart6_buffer[UART6RINGBUFFER_SIZE];

#define UART6DMABUFFERSIZE       256
static uint8_t uart6_dma[UART6DMABUFFERSIZE];

static void HAL_UART6_TxCpltCallback(UART_HandleTypeDef *huart)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	if(uart6_sem != NULL) {
		xSemaphoreGiveFromISR(uart6_sem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	
}

static void HAL_UART6_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_DMAStop(huart);
	uint16_t lenght = UART6DMABUFFERSIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
	ringbuffer_put(&uart6_ring, uart6_dma, lenght);
	HAL_UART_Receive_DMA(huart, uart6_dma, UART6DMABUFFERSIZE);
}

void hal_uart_irq(void)
{
	if(__HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart6);
        
        HAL_UART_DMAStop(&huart6);
        __HAL_UART_FLUSH_DRREGISTER(&huart6);
        
        uint16_t lenght = UART6DMABUFFERSIZE - __HAL_DMA_GET_COUNTER(huart6.hdmarx);
        // put data to ringbuffer
        ringbuffer_put(&uart6_ring, uart6_dma, lenght);
    
        HAL_UART_Receive_DMA(&huart6, uart6_dma, UART6DMABUFFERSIZE);
    }
}

void hal_uart_init(void)
{
	ringbuffer_init(&uart6_ring, uart6_buffer, UART6RINGBUFFER_SIZE);

	uart6_sem = xSemaphoreCreateBinary();

	HAL_UART_RegisterCallback(&huart6, HAL_UART_TX_COMPLETE_CB_ID, HAL_UART6_TxCpltCallback);
	HAL_UART_RegisterCallback(&huart6, HAL_UART_RX_COMPLETE_CB_ID, HAL_UART6_RxCpltCallback);

	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
    //__HAL_UART_ONE_BIT_SAMPLE_ENABLE(&huart3);

	HAL_UART_Receive_DMA(&huart6, uart6_dma, UART6DMABUFFERSIZE);
}

uint32_t hal_uart_write(void *data, uint32_t size)
{
	while(huart6.gState != HAL_UART_STATE_READY);
    
    if(HAL_UART_Transmit_DMA(&huart6, (uint8_t *)data, size) != HAL_OK) {
        return 0;
    }
    xSemaphoreTake(uart6_sem, 1000);

	return size;
}

uint32_t hal_uart_read(void *data, uint32_t size)
{
	return ringbuffer_get(&uart6_ring, (uint8_t *)data, size);
}

uint32_t hal_uart_valid(void)
{
	return ringbuffer_data_len(&uart6_ring);
}
