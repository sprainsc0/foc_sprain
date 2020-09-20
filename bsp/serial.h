#ifndef UART_EX_H
#define UART_EX_H

#ifdef __cplusplus
extern "C"{
#endif

#include "datatypes.h"
#include "cmsis_os.h"
#include "usart.h"

void hal_uart_init(void);
uint32_t hal_uart_write(void *data, uint32_t size);
uint32_t hal_uart_read(void *data, uint32_t size);
uint32_t hal_uart_valid(void);
void hal_uart_irq();
#ifdef __cplusplus
}
#endif
#endif

