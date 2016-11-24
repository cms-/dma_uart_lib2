#ifndef SYSTICK_H
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/f0/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/cortex.h>

#define BUFFERSIZE 64
#define WORDBYTES 4
extern int32_t SerTXSize;
extern int32_t SerTXDMA;
extern int32_t SerTXIRQ;

extern int32_t SerRXSize;
extern int32_t SerRXDMA;
extern int32_t SerRXIRQ;
typedef enum { FIFO_TX = 0, FIFO_RX } fifo_direction_e;
typedef struct {
	uint32_t data[BUFFERSIZE]; // number of elements in buffer
	uint32_t *getPt; 
	uint32_t *putPt;
	uint32_t size;
	int32_t *dma_flag;
	int32_t *size_flag;
	int32_t *irq_flag;
	fifo_direction_e dir;
	void (*handler_function)(volatile void *data, int length);
	volatile uint8_t bufRcv[4];
	//void(*handler)(volatile void*, int)
} fifo_t;
extern fifo_t SerTXFifo[1];
extern fifo_t SerRXFifo[1];
extern fifo_t TestingFifo[1];
//extern volatile uint8_t BufRcv[4];

// Buffer management event table
struct event_t {
	void(*managerFunction)(fifo_t*); // manager function is called with reference to fifo being managed
	fifo_t *fifo; // pointer to FIFO buffer
	uint32_t interval; // how often the manager function will be called in microseconds
	uint32_t last; // last executed
};
typedef struct event_t eventType;

// ******* Systick_Init *******
// Initializes Systick timer and system management
// functionality.
// Inputs/Ouputs: none
void Sys_Init(void);

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
// Inputs: pointer to a manager function
//         period in milliseconds
//         pointer to a fifo type
// Outputs: nothing
void Sys_AddPeriodicEvent(void(*function)(fifo_t*), uint32_t period_ms, fifo_t *fifo);

// ******* runPeriodicEvents *******
// Runs periodic event scheduler
// Inputs/Outputs: none
void static run_periodic_events(void);

// ******* FifoInit *******
// Initializes a FIFO structure, preparing it for usage.
// Inputs: pointer to fifo_t
//         number of elements in the buffer <- follow-up: may not be necessary
//		   pointer to a DMA flag
//		   pointer to a buffer size flag
//		   pointer to an irq flag
//		   pointer to a handler function (handler is called by qxx_manager)
//		   direction enumerator FIFO_TX or FIFO_RX
// Ouputs: None
void FifoInit(fifo_t *fifo, uint32_t size, int32_t *dma_flag, int32_t *size_flag, int32_t *irq_flag, void(*handler)(volatile void *data, int length), fifo_direction_e dir);

// ******* FifoPut *******
// Appends a length of data to a supplied buffer.
// Inputs: pointer to data, pointer to a fifo_t, data length
// Outputs: number of data inserted successfully
extern uint32_t FifoPut(volatile void *data, fifo_t *fifo, uint32_t length);

// ******* FifoPeek *******
// Retrieves data from a specified buffer non-destructively.
// Inputs: data pointer, fifo_t pointer, and data length.
// Outputs: number of data retrieved
extern uint32_t FifoPeek(void *data, fifo_t *fifo, uint32_t length);

// ******* FifoDelete *******
// Removes a specified number of elements from the fifo_t provided
// Inputs: fifo_t pointer, length to delete
// Outputs: none
extern void FifoDelete(fifo_t *fifo, uint32_t length);

// ******* FifoGet *******
// Retrieves and removes data from a specified buffer.
// Inputs: data pointer, fifo_t pointer, and data length.
// Outputs: none
extern uint32_t FifoGet(volatile void *data, fifo_t *fifo, uint32_t length);

// ******* qtx_manager *******
// Periodic event that manages transmit queue.
// Inputs: fifo_t pointer
// Outputs: none
void static qtx_manager(fifo_t *fifo);

// ******* qrx_manager *******
// Periodic event that manages receive queue.
// Inputs: fifo_t pointer
// Outputs: none
void static qrx_manager(fifo_t *fifo);

void static dummy_event(void *foo);

void static test_event(void *foo);

#define SYSTICK_H 1
#endif