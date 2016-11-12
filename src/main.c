#include "uart.h"
#include "systick.h"
#define PORT_LED GPIOA
#define PIN_LED GPIO10

int main(void)
{

	uint8_t i;
	int j;

	rcc_init();
	gpio_init();
	nvic_init();
	dma_init();
	uart_init();
	Sys_Init();
	Uart_send("Hello",5);
	/*
	for (i=0; i<555555555; i++)
	{
		//for (j=0; j<10000; j++);
		Uart_send("H", 1);	
	} // delay
	*/
	while (1) {
		for (j=0; j<10000000; j++);
		//Uart_send("Hello",5);
		
		
		//uint8_t i = usart_recv(USART1);
		//gpio_set(PORT_LED, PIN_LED);
		for (j=0; j<1000000; j++);
		//Uart_send("Mexico",6);
		//usart_send_blocking(USART2, 'D');
		//uart_send("Hello World!", 12);
			//gpio_clear(PORT_LED, PIN_LED);

	}

	return 0;
}