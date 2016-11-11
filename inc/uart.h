#ifndef UART_H
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
//#include <libopencm3/stm32/nvic.h>
#include <libopencm3/stm32/f0/nvic.h>



//void uart_receive(volatile void* data, int length);
void Uart_send(volatile void* data, int length);

// ******* uart_qtx_dma *******
// Function that transfers a series of 8 bit values from 
// the transmit queue to the UART DMA. 
// Inputs: pointer to a series of 8 bit values
//         number of values to be read from data point
// Outputs: none
extern void uart_qtx_dma(volatile void *data, int length);

// ******* uart_qrx_dma *******
// Function that transfers a series of 8 bit values from 
// the UART DMA to the receive queue.
// Inputs: pointer to a series of 8 bit values
//         number of values to be read from DMA channel
// Outputs: none
extern void uart_qrx_dma(volatile void *data, int length);

void rcc_init(void);
void gpio_init(void);
void uart_init(void);
void dma_init(void);
void nvic_init(void);

#define UART_H 1
#endif