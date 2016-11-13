#include "uart.h"
#include "systick.h"

// Uart_Send
// Public function that sends a series of bytes
// via the serial FIFO queue.
// Inputs: pointer to a series of 8 bit values
//         number of 8 bit values to be read from data pointer
// Ouputs: none
void Uart_send(volatile void* data, int length) {
	//while (!SerTXFifo->dma_flag);
	FifoPut(data, SerTXFifo, length);

}

// ******* uart_qtx_dma *******
// Function that transfers a series of 8 bit values from 
// the transmit queue to the UART DMA. 
// Inputs: pointer to a series of 8 bit values
//         number of values to be read from data point
// Outputs: none
void uart_qtx_dma(volatile void *data, int length)
{
	dma_disable_channel(DMA1, DMA_CHANNEL2);

	dma_set_memory_address(DMA1, DMA_CHANNEL2, (uint32_t) data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL2, length);

	dma_enable_channel(DMA1, DMA_CHANNEL2);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL2);
	usart_enable_tx_dma(USART1);
}

// ******* uart_qrx_dma *******
// Function that transfers a series of 8 bit values from 
// the UART DMA to the receive queue.
// Inputs: pointer to a series of 8 bit values
//         number of values to be read from DMA channel
// Outputs: none
void uart_qrx_dma(volatile void *data, int length)
{
	dma_disable_channel(DMA1, DMA_CHANNEL3);

	//dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);

	dma_set_memory_address(DMA1, DMA_CHANNEL3, (uint32_t) data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL3, length);

	dma_enable_channel(DMA1, DMA_CHANNEL3);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL3);
	usart_enable_rx_dma(USART1);
}

void rcc_init() {
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_GPIOA);
	//rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_DMA);
}

void gpio_init() {
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO2|GPIO3);
}

void uart_init() {
	//USART1_CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	//usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_enable_rx_interrupt(USART1);
	//usart_disable_tx_interrupt(USART1);
    //usart_disable_error_interrupt(USART1);
	usart_enable(USART1);

}

void dma_init() {
	dma_channel_reset(DMA1, DMA_CHANNEL2);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL2, (uint32_t) &USART1_TDR);
	dma_set_read_from_memory(DMA1, DMA_CHANNEL2);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL2);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL2, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL2, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL2, DMA_CCR_PL_VERY_HIGH);
	
	dma_channel_reset(DMA1, DMA_CHANNEL3);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL3, (uint32_t) &USART1_RDR);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL3);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL3);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL3, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL3, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL3, DMA_CCR_PL_VERY_HIGH);
	//gpio_set(GPIOA, GPIO10);
	
}

void nvic_init() {
	nvic_set_priority(NVIC_DMA1_CHANNEL2_3_IRQ, 2);
	nvic_enable_irq(NVIC_DMA1_CHANNEL2_3_IRQ);
  /* Configure interrupt controller */
	//NVIC_ISER(0) |= (1<<NVIC_USART1_IRQ);
	//USART1_RQR |= USART_RQR_RXFRQ;
	nvic_set_priority(NVIC_USART1_IRQ, 4);
	nvic_enable_irq(NVIC_USART1_IRQ);
}

void dma1_channel2_3_isr(void) {
	uint32_t isr = DMA1_ISR;
	//uart_qtx_dma(&isr, 4);
	//gpio_toggle(GPIOA, GPIO10); /* LED2 on/off */
	//if (isr & (1 << 5))
	if (isr & DMA_ISR_TCIF2)
	{
		//gpio_toggle(GPIOA, GPIO10); /* LED2 on/off */
		DMA1_IFCR |= DMA_IFCR_CTCIF2;	//Clear flag
		dma_disable_channel(DMA1, DMA_CHANNEL2);
		Sys_Signal(&SerTXFifo->dma_flag);
		
	}

	if (isr & DMA_IFCR_TCIF3)
	{
		//gpio_toggle(GPIOA, GPIO10);
		DMA1_IFCR |= DMA_IFCR_CTCIF3;	//Clear flag
		dma_disable_channel(DMA1, DMA_CHANNEL3);
		Sys_Signal(&SerRXFifo->dma_flag);
	}

}

void usart1_isr(void)
{
	
    uint32_t isr = USART1_ISR;
    //uart_qtx_dma(&isr, 1);
    if (isr & USART_ISR_RXNE)
    //if (USART1_ISR &= USART_ISR_RXNE) {
    {
    	// incoming serial data
    	// first clear the interupt and then signal the irq flag
    	USART1_RQR |= USART_RQR_RXFRQ;
    	Sys_Signal(&SerRXFifo->irq_flag)
    	//uint8_t i = usart_recv(USART1);
        //gpio_clear(GPIOA, GPIO10);
    }
    if (isr &= USART_ISR_TXE) {
        //do_usart1_tx();
        //gpio_set(GPIOA, GPIO10);
    }
}