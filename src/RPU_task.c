//freeRTOS

#include "RPU_task.h"





/********************task's functions ********************/


void activeWaiting(const int ms){
	long fn,f1,f2;
	int n = 100000;
	for (int i = 0; i<(ms/45);i++){
		f1 = 2;
		f2 =4;
		fn = 5;
		// this function takes about 45 mseconds (Worst Case)
		for (int j=0; j<100000;j++){
			fn = fn % 1;
			f1 = f1  % 1;
			f2 = f2 % 1;
		}
	}
}

void periodicTask1(void *unused_arg)
{
	 LPRINTF("[RPU]: Periodic Task 1 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 const TickType_t xFrequency = 15*45;
	 int first = 1;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();
	         LPRINTF("[PT1]:StartTime: %d\n", start);
	         activeWaiting(5*45);

	         end = xTaskGetTickCount();
	         LPRINTF("[PT1]:EndTime: %d\n", end);


	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 1 DEADLINE EXCEEDED \n");
	         if(first==1)first = 0;
	     }
}

void periodicTask2(void *unused_arg)
{
	 LPRINTF("[RPU]: Periodic Task 2 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 const TickType_t xFrequency = 40*45;
	 int first = 1;
	 int i=0;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	    	 i++;

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();
	         LPRINTF("[PT2]:StartTime: %d\n", start);
	         activeWaiting(10*45);
	         end = xTaskGetTickCount();
	         LPRINTF("[PT2]:EndTime: %d\n", end);

	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 2 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}

void periodicTask3(void *unused_arg)
{
	 LPRINTF("[RPU]: Periodic Task 3 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 const TickType_t xFrequency = 90*45;
	 int first = 1;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();
	         LPRINTF("[PT3]:StartTime: %d\n", start);
	         activeWaiting(15*45);
	         end = xTaskGetTickCount();
	         LPRINTF("[PT3]:EndTime: %d\n", end);


	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 3 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}

void periodicTask4(void *unused_arg)
{
	// LPRINTF("[RPU]: Periodic Task 4 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 const TickType_t xFrequency = 300*45;
	 int first = 1;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();
	         LPRINTF("[PT4]:StartTime: %d\n", start);
	         activeWaiting(30*45);
	         end = xTaskGetTickCount();
	         LPRINTF("[PT4]:EndTime: %d\n", end);

	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 4 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}

void periodicTask5(void *unused_arg)
{
	// LPRINTF("[RPU]: Periodic Task 5 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 TickType_t WCET,media;
	 const TickType_t xFrequency = 600*45;
	 int first = 1;
	 int i=0;
	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();
	         LPRINTF("[PT5]:StartTime: %d\n", start);
	         activeWaiting(60*45);
	         end = xTaskGetTickCount();
	         LPRINTF("[PT5]:ExTime: %d\n", end);


	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 5 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}


void aperiodicTask1(void *unused_arg)
{

	 LPRINTF("[ATASK1]: ap start: %d \n", xTaskGetTickCount());
	 uint32_t start = 0;
	 uint32_t end = 0;
	 const int N = 2;
	 int fn, fn1, fn2;

	 fn1 = 1;
	 fn2 = 1;
	 fn = 0;

	 for (int i = 0; i< N; i++ ){

		 if ( i > 1 ){
			 fn = fn1 + fn2;
			 fn2 = fn1;
			 fn1 = fn;
		 }

		 activeWaiting(25*45);

	 }
	 
	 LPRINTF("[ATASK1] end: %d\n", xTaskGetTickCount());
	 deleteATask();
}


