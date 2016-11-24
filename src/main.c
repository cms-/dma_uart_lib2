#include "uart.h"
#include "systick.h"
#define PORT_LED GPIOA
#define PIN_LED GPIO10

int main(void)
{

	int i;
	int16_t j;

	rcc_init();
	gpio_init();
	nvic_init();
	dma_init();
	uart_init();
	Sys_Init();
	Uart_send("Hello",5);
	for (i=0; i<10000000; i++);
	//gpio_set(GPIOF, GPIO1);
	/*
	for (i=0; i<555555555; i++)
	{
		//for (j=0; j<10000; j++);
		Uart_send("H", 1);	
	} // delay
	*/
	while (1) {
		//for (j=0; j<10000000; j++);
		//Uart_send("Hello",5);
		
		//j = gpio_get(GPIOA, GPIO1);
		//j=0;
		//Uart_send(&j, 1);
		//if (j == 0x02)
		//{
		//	gpio_set(PORT_LED, PIN_LED);
		//}
		//else
		//{
		//	gpio_clear(PORT_LED, PIN_LED);
		//}

		//for (i=0; i<1000000; i++);
		uint32_t buf;
		uint32_t len;
	
		if ((len = FifoGet(&buf, SerRXFifo, 4)))
		{
			FifoPut(&buf, SerTXFifo, len);
		}

		//Uart_send("Mexico",6);
		//usart_send_blocking(USART2, 'D');
		//uart_send("Hello World!", 12);
		//gpio_clear(PORT_LED, PIN_LED);

	}

	return 0;
}