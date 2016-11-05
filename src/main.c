#include "uart.h"
#include "systick.h"
#define PORT_LED GPIOA
#define PIN_LED GPIO10

int main(void)
{

	volatile int i;

	rcc_init();
	gpio_init();
	nvic_init();
	uart_init();
	dma_init();
	Systick_Init();

	while (1) {

		//gpio_toggle(PORT_LED, PIN_LED);	/* LED on/off */
		for (i=0; i<1000; i++); // delay
		Uart_send("Hello", 5);
		//usart_send_blocking(USART2, 'D');
		//uart_send("Hello World!", 12);

	}

	return 0;
}