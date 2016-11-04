#ifndef UART_H
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/f0/nvic.h>



//void uart_receive(volatile void* data, int length);
void Uart_send(volatile void* data, int length);

// ******* uart_queue *******
// Queues a byte in the FIFO transmit queue
// Inputs: pointer to 8 bit value
// Outputs: none
extern void uart_qtx_dma(volatile void *data, int length);

void rcc_init(void);
void gpio_init(void);
void uart_init(void);
void dma_init(void);
void nvic_init(void);

#define UART_H 1
#endif