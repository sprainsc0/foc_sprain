#include "serial.h"
#include "ringbuffer.h"
#include <semphr.h>

static struct ringbuffer uart3_ring;
static SemaphoreHandle_t uart3_sem;
#define UART3RINGBUFFER_SIZE     1024
static uint8_t uart3_buffer[UART3RINGBUFFER_SIZE];

#define UART3DMABUFFERSIZE       256
static uint8_t uart3_dma[UART3DMABUFFERSIZE];

static void HAL_UART3_TxCpltCallback(UART_HandleTypeDef *huart)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	if(uart3_sem != NULL) {
		xSemaphoreGiveFromISR(uart3_sem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	
}

static void HAL_UART3_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_DMAStop(huart);
	uint16_t lenght = UART3DMABUFFERSIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
	ringbuffer_put(&uart3_ring, uart3_dma, lenght);
	HAL_UART_Receive_DMA(huart, uart3_dma, UART3DMABUFFERSIZE);
}

void hal_uart_irq(void)
{
	if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);
        
        HAL_UART_DMAStop(&huart3);
        __HAL_UART_FLUSH_DRREGISTER(&huart3);
        
        uint16_t lenght = UART3DMABUFFERSIZE - __HAL_DMA_GET_COUNTER(huart3.hdmarx);
        // put data to ringbuffer
        ringbuffer_put(&uart3_ring, uart3_dma, lenght);
    
        HAL_UART_Receive_DMA(&huart3, uart3_dma, UART3DMABUFFERSIZE);
    }
}

void hal_uart_init(void)
{
	ringbuffer_init(&uart3_ring, uart3_buffer, UART3RINGBUFFER_SIZE);

	uart3_sem = xSemaphoreCreateBinary();

	HAL_UART_RegisterCallback(&huart3, HAL_UART_TX_COMPLETE_CB_ID, HAL_UART3_TxCpltCallback);
	HAL_UART_RegisterCallback(&huart3, HAL_UART_RX_COMPLETE_CB_ID, HAL_UART3_RxCpltCallback);

	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    //__HAL_UART_ONE_BIT_SAMPLE_ENABLE(&huart3);

	HAL_UART_Receive_DMA(&huart3, uart3_dma, UART3DMABUFFERSIZE);
}

uint32_t hal_uart_write(void *data, uint32_t size)
{
	while(huart3.gState != HAL_UART_STATE_READY);
    
    if(HAL_UART_Transmit_DMA(&huart3, (uint8_t *)data, size) != HAL_OK) {
        return 0;
    }
    xSemaphoreTake(uart3_sem, 1000);

	return size;
}

uint32_t hal_uart_read(void *data, uint32_t size)
{
	return ringbuffer_get(&uart3_ring, (uint8_t *)data, size);
}

uint32_t hal_uart_valid(void)
{
	return ringbuffer_data_len(&uart3_ring);
}
