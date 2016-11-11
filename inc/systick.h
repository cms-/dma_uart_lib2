#ifndef SYSTICK_H
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/f0/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/cortex.h>

#define BUFFERSIZE 512
extern int32_t SerTXSize;
extern int32_t SerTXDMA;
extern int32_t SerRXSize;
extern int32_t SerRXDMA;
typedef struct {
	uint32_t data[BUFFERSIZE];
	uint32_t *getPt;
	uint32_t *putPt;
	uint32_t size;
	int32_t *dma_flag;
	int32_t *size_flag;
	int32_t *irq_flag;
	void (*handler_function)(void *data, uint32_t length);
} fifo_t;
extern fifo_t SerTXFifo[1];
extern fifo_t SerRXFifo[1];

// Buffer management event table
struct event_t {
	void(*function)(void *fifo); // manager function is called with reference to fifo being managed
	fifo_t *fifo; // pointer to FIFO buffer
	uint32_t interval; // how often the manager function will be called in microseconds
	uint32_t last; // last executed
};
typedef struct event_t eventType;

// ******* Systick_Init *******
// Initializes Systick timer
// Inputs/Ouputs: none
void Systick_Init(void);

// ******* Sys_InitSema *******
// Initialize a counting semaphore
// Inputs: pointer to a semaphore
//         initial value of semaphore
// Outputs: none
void Sys_InitSema(int32_t *semaPt, int32_t value);

// ******* Sys_Wait *******
// Decrement semaphore, blocking task if less than zero
// Inputs: pointer to a counting semaphore
void Sys_Wait(int32_t *semaPt);

// ******* Sys_Signal *******
// Increment semaphore
// Inputs: pointer to a counting semaphore
void Sys_Signal(int32_t *semaPt);

// ******* Sys_AddPeriodicEvent *******
// Adds event to period event table
// Inputs: pointer to a function
//         period in microseconds
//         counting semaphore for DMA status
//         counting semaphore for buffer size
// Outputs: nothing
void Sys_AddPeriodicEvent(void(*function)(void*), uint32_t period_ms, fifo_t *fifo);

// ******* runPeriodicEvents *******
// Runs periodic event scheduler
// Inputs/Outputs: none
void static run_periodic_events(void);

void FifoInit(fifo_t *fifo, uint32_t size, int32_t *dma_flag, int32_t *size_flag);

// ******* FifoPut *******
// Appends a word of data to the global FIFO
// Inputs: pointer to a  
// Outputs: 0 for success
extern uint32_t FifoPut(volatile void *data, fifo_t *fifo, uint32_t length);

// ******* FifoPeek *******
// Runs periodic event scheduler
// Inputs/Outputs: none
extern uint32_t FifoPeek(void *data, fifo_t *fifo, uint32_t length);

// ******* FifoDelete *******
// Runs periodic event scheduler
// Inputs/Outputs: none
extern void FifoDelete();

// ******* FifoGet *******
// Runs periodic event scheduler
// Inputs/Outputs: none
extern uint32_t FifoGet(volatile void *data, fifo_t *fifo, uint32_t length);

// ******* uart_txqueue_manager *******
// Periodic event that manages the uart
// transmit queue.
// Inputs/Outputs: none
void static qtx_manager(fifo_t *fifo);

void static qrx_manager(fifo_t *fifo);

void static dummy_event(void *foo);

void static test_event(void *foo);

#define SYSTICK_H 1
#endif