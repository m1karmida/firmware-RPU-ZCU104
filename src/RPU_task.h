
#ifndef _RPU_TASK.H
#define _RPU_TASK.H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//OpenAMP-Libmetal
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "rsc_table.h"
#include "platform_info.h"
#include "rpmsg-echo.h"
//Xilinx
#include "xil_printf.h"

//Print end errors
#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

//Number of existing Task on RPU
#define RPU_N_TASK 6
#define RPU_N_P_TASK 5

//Task definition
#define TASK_1		0X00000001
#define STOP_TASK_1	0X00010000
#define TASK_2 		0x00000002
#define STOP_TASK_2	0X00020000
#define TASK_3		0X00000003
#define STOP_TASK_3	0X00030000
#define TASK_4 		0x00000004
#define STOP_TASK_4	0X00040000
#define TASK_5 		0x00000005
#define STOP_TASK_5	0X00050000


// aperiodic task definition
#define ATASK_1 		0x00000006
#define STOP_ATASK_1	0X00060000




//Task handle definitions
static TaskHandle_t perodic_task_handle1;
static TaskHandle_t perodic_task_handle2;
static TaskHandle_t perodic_task_handle3;
static TaskHandle_t perodic_task_handle4;
static TaskHandle_t perodic_task_handle5;
static TaskHandle_t aperodic_task_handle1;


struct RPU_Task{
	uint32_t code;						// Code needed to require the Task (same on APU)
	uint32_t stop_code;					// Code needed to delete the Task  (same on APU)
	uint32_t load;						// Task load
	TaskFunction_t function;			// Task function interface name
	char name[30];						// Name of the function
	UBaseType_t priority;				// Priority of the Task
	TaskHandle_t const * Handler;		// Task handler Name
	int running;						// Task is currently running
};


/***** tasks function prototypes ******/

void activeWaiting(const int ms);

void periodicTask1(void *unused_arg);

void periodicTask2(void *unused_arg);

void periodicTask3(void *unused_arg);

void periodicTask4(void *unused_arg);

void periodicTask5(void *unused_arg);

void aperiodicTask1(void *unused_arg);

/* The load of Sporadic server is 10%
 * The execution of the first two tasks is schedulable with Rate Monotonic.
 * The execution of the first three tasks is unschedulable with RM but they are under 100% of CPU utilization.
 * The execution of the first five tasks is unschedulable.
*/

static const int WCET = 50*45;
/*RPU task
 * NOTE: the second parameter for periodic task represent the percentage utilization factor
 * 		 for aperiodic tasks represent the worst case execution time because the load is not needed for them (handled by SS's load)
 * */

static struct RPU_Task Task_vector[RPU_N_TASK] =
{
	{TASK_1, STOP_TASK_1, 34, periodicTask1, "periodicTask1", 7, &perodic_task_handle1, 0},		//Task 1 T=15
	{TASK_2, STOP_TASK_2, 25, periodicTask2, "periodicTask2", 6, &perodic_task_handle2, 0},		//Task 2 T=40
	{TASK_3, STOP_TASK_3, 17, periodicTask3, "periodicTask3", 5, &perodic_task_handle3, 0},		//Task 3 T=90
	{TASK_4, STOP_TASK_4, 20, periodicTask4, "periodicTask4", 3, &perodic_task_handle4, 0},		//Task 4 T=100
	{TASK_5, STOP_TASK_5, 10, periodicTask5, "periodicTask5", 2, &perodic_task_handle5, 0},		//Task 5 T=300


	{ATASK_1, STOP_ATASK_1, WCET, aperiodicTask1, "aperiodicTask1", 4, &aperodic_task_handle1, 0}
};


#endif
