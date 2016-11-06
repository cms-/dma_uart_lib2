#include "uart.h"
#include "systick.h"
#define PORT_LED GPIOA
#define PIN_LED GPIO10

int main(void)
{

	int i;
	int j;

	rcc_init();
	gpio_init();
	nvic_init();
	uart_init();
	dma_init();
	Systick_Init();
	//Uart_send("Hello",5);
	/*
	for (i=0; i<555555555; i++)
	{
		//for (j=0; j<10000; j++);
		Uart_send("H", 1);	
	} // delay
	*/
	while (1) {
		for (j=0; j<1000000; j++);
		Uart_send("Hello",5);
		
		for (j=0; j<1000000; j++);
		Uart_send("Mexico",6);
		//usart_send_blocking(USART2, 'D');
		//uart_send("Hello World!", 12);

	}

	return 0;
}