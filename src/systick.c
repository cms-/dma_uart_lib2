#include "systick.h"
#include "uart.h"

#define NUMEVENTS 1

fifo_t SerTXFifo[1];
uint8_t SerTXBuffer[TXBUFFERSIZE];
int32_t SerTXDMA;
int32_t SerTXSize;
uint32_t TheTime = 0;

eventType events[NUMEVENTS];

int32_t tmp = 1;

// ******* uart_txqueue_manager *******
// Periodic event that manages the uart
// transmit queue.
// Inputs: pointer to tx fifo
// Outputs: none
void static qtx_manager(fifo_t *fifo)
{
	uint32_t buf;
	uint32_t len;
	
	if ((len = FifoGet(&buf, fifo, 1)))
	{
		Sys_Wait(fifo->dma_flag);
		uart_qtx_dma(&buf, len); // fold this into fifo block
		
		//FifoDelete(fifo, 1);
	}
}

void static test_event(void *foo)
{
	gpio_toggle(GPIOA, GPIO10); /* LED2 on/off */
}

void static dummy_event(void *foo)
{
	int j;
	for (j=0; j<1000; j++);
}

void Systick_Init(void) {
	/* 72MHz / 8 => 9000000 counts per second */
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);

	/* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(47999);

	systick_interrupt_enable();
	/* Start counting. */
	systick_counter_enable();
	Sys_InitSema(&SerTXDMA, 1);
	Sys_InitSema(&SerTXSize, 0);
	FifoInit(SerTXFifo, (void*)SerTXBuffer, sizeof(SerTXBuffer), &SerTXDMA, &SerTXSize);
	Sys_AddPeriodicEvent(&test_event, 1000, SerTXFifo);
	//Sys_AddPeriodicEvent(&dummy_event, 100000, &tmp, &tmp);
}

void sys_tick_handler(void)
{
	run_periodic_events();
	TheTime++;
}

// ******* SysInitSema *******
// Initialize a counting semaphore
// Inputs: pointer to a semaphore
//         initial value of semaphore
// Outputs: none
void Sys_InitSema(int32_t *semaPt, int32_t value) 
{
	(*semaPt) = value;

}

// ******* SysWait *******
// Decrement semaphore, blocking task if less than zero
// Inputs: pointer to a counting semaphore
void Sys_Wait(int32_t *semaPt) 
{
	(*semaPt)--;

}

// ******* SysSignal *******
// Increment semaphore
// Inputs: pointer to a counting semaphore
void Sys_Signal(int32_t *semaPt)
{
	(*semaPt)++;
}

// ******* Sys_AddPeriodicEvent *******
// Adds event to period event table
// Inputs: pointer to a function
//         period in microseconds
//         counting semaphore for DMA status
//         counting semaphore for buffer size
// Outputs: nothing
void Sys_AddPeriodicEvent(void(*function)(void*), uint32_t period_ms, fifo_t *fifo)
{
	int j;
	for (j=0; j<NUMEVENTS; j++)
	{
		if (!events[j].function)
		{
			events[j].function = (function);
			events[j].interval = period_ms;
			events[j].last = 0;
			events[j].fifo = fifo;
			break;
		}
	}
}

// ******* RunPeriodicEvents *******
// Runs periodic event scheduler
// Inputs/Outputs: none
void static run_periodic_events(void)
{
	cm_disable_interrupts();
	int j;
	for (j=0; j<NUMEVENTS; j++)
	{
		if (((TheTime - events[j].last) >= events[j].interval) && ((events[j].fifo->size_flag) && (events[j].fifo->dma_flag)) )  
		{
			events[j].function(events[j].fifo);
			events[j].last = TheTime;
		}
	}
	TheTime++;
	cm_enable_interrupts();
}

void FifoInit(fifo_t *fifo, uint8_t *data, uint32_t size, int32_t *dma_flag, int32_t *size_flag)
{
	fifo->getPt = (uint32_t*) &data[0];
	fifo->putPt = (uint32_t*) &data[0];
	fifo->size = size;
	fifo->data = (uint32_t*) &data;
	fifo->dma_flag = dma_flag;
	fifo->size_flag = size_flag;

}

// ******* FifoPut *******
// Appends a word of data to the global FIFO
// Inputs: pointer to a 32 bit word 
// Outputs: 0 for success
uint32_t FifoPut(volatile void *data, fifo_t *fifo, uint32_t length)
{
	cm_disable_interrupts();
	int j;
	char *p;
	p = data;
	for (j=0; j < length; j++)
	{
		// Check to see if there's space in the buffer
		if ( (fifo->putPt + 1 == fifo->getPt) ||
			( (fifo->putPt + 1 == &fifo->data[-1]) && (fifo->getPt == &fifo->data[0]) ))
		{
			//gpio_toggle(GPIOA, GPIO10); /* LED2 on/off */
			cm_enable_interrupts();
			return j; // no room left
		}
		else
		{
			(*fifo->putPt) = *p++;
			Sys_Signal(fifo->size_flag);
			//(*p)++;
			fifo->putPt++; // advance to next buffer slot
			if ( fifo->putPt == &fifo->data[-1] )
			{
				fifo->putPt = (uint32_t*) fifo->data[0];
			}

		}
		
	}
	cm_enable_interrupts();
	return j;
}

// ******* FifoPeek *******
// Runs periodic event scheduler
// Inputs/Outputs: none
uint32_t FifoPeek(void *data, fifo_t *fifo, uint32_t length)
{
	cm_disable_interrupts();
	uint32_t j;
	char *p;
	uint32_t *tmpGet;
	tmpGet = (uint32_t*) fifo->getPt;
	p = data;

	for (j=0; j < length; j++) {
		
		if (tmpGet != (uint32_t*)&fifo->putPt)
		//if (fifo->tail != fifo->head)
		{
			*p++ = (*tmpGet);
			//*p++ = fifo->data[fifo->tail];
			//uart_qtx_dma(fifo->data[fifo->tail], sizeof(fifo->data[fifo->tail]));
			
			tmpGet++;
			//fifo->tail++;
			if (tmpGet == &fifo->data[-1])
			//if (fifo->tail == fifo->size)
			{
				tmpGet = (uint32_t*) &fifo->data[0];
			}

		}
		else
		{
			cm_enable_interrupts();
			
			return j;
		}
	}
	cm_enable_interrupts();
	return j;
}

// ******* FifoDelete *******
// Runs periodic event scheduler
// Inputs/Outputs: none
void FifoDelete(fifo_t *fifo, uint32_t length) 
{
	cm_disable_interrupts();
	//return 0;
	cm_enable_interrupts();
}

// ******* FifoGet *******
// Runs periodic event scheduler
// Inputs/Outputs: none
uint32_t FifoGet(volatile void *data, fifo_t *fifo, uint32_t length)
{
	cm_disable_interrupts();
	uint32_t j;
	char *p;
	uint32_t *tmpGet;
	tmpGet = (uint32_t*) fifo->getPt;
	p = data;

	for (j=0; j < length; j++) {
		
		//if (tmpTail != fifo->head)
		if (fifo->getPt != fifo->putPt)
		{
			//*p++ = fifo->data[tmpTail];
			*p++ = (*fifo->getPt);
			//uart_qtx_dma(fifo->data[fifo->tail], sizeof(fifo->data[fifo->tail]));
			
			//tmpTail++;
			fifo->getPt++;
			Sys_Wait(fifo->size_flag);
			//if (tmpTail == fifo->size)
			if (fifo->getPt == (uint32_t*)&fifo->data[-1])
			{
				fifo->getPt = (uint32_t*)&fifo->data[0];
			}

		}
		else
		{
			cm_enable_interrupts();
			
			return j;
		}
	}
	cm_enable_interrupts();
	return j;
}

