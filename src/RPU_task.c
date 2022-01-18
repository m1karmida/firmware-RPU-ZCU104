//freeRTOS

#include "RPU_task.h"





/********************task's functions ********************/


void activeWaiting(const int ms){
//	TickType_t time1 = xTaskGetTickCount();
//	while(xTaskGetTickCount() < time1 + pdMS_TO_TICKS(ms));
	TickType_t start,end;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t xFrequency = 15;
    long j=0;
  long zero = 0;
  long uno = 0;
    int f,f1,f2;
    f = 1;
    f1 = 2;
    f2 = 3;

    TickType_t WCET, media;
    WCET = 0;
    media = 0;

    for( int i = 0;i<1000;i++)
    {
        // Wait for the next cycle.
   	start = xTaskGetTickCount();
    	j = 0;
        while(xTaskGetTickCount()<start+1){
        	f = f %1;
        	f1 = f1 % 1;
        	f2 = f2 % 1;
        	j++;
        }

        LPRINTF("j = %d \n", j);
        if (WCET < j)
        	WCET = j;
        media += j;

     //   if ((end-start)==0)
    //    	zero++;
   //     else if ((end-start)==1)
  //      	uno++;

	}
    media /= 1000;
    LPRINTF("WCET: %d  \t media : %d  \n",WCET,media);
    vTaskDelete(NULL);

}

void periodicTask1(void *unused_arg)
{
	 LPRINTF("[RPU]: Periodic Task 1 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 const TickType_t xFrequency = 15;
	 int first = 1;
	 int i = 0;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {	 i++;
	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();

	         LPRINTF("[PTASK1] Periodic Task 1 start its execution at time: %d \n", pdTICKS_TO_MS(xTaskGetTickCount()));

	         activeWaiting(10*5);

	         LPRINTF("[PTASK1] Periodic Task 1 terminate its execution at time: %d \n", pdTICKS_TO_MS(xTaskGetTickCount()));

	         end = xTaskGetTickCount();
//	         if (i%50  == 0)
	    //     LPRINTF("[PT1]: %d\n", end-start);
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
	 const TickType_t xFrequency = 40;
	 int first = 1;
	 int i=0;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	    	 i++;
	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();

	         LPRINTF("[PTASK2] Periodic Task 2 start its execution at time: %d \n", pdTICKS_TO_MS(xTaskGetTickCount()));

	         activeWaiting(10*10);

	         LPRINTF("[PTASK2] Periodic Task 2 terminate its execution at time: %d \n", pdTICKS_TO_MS(xTaskGetTickCount()));

	         end = xTaskGetTickCount();
	         //if (i%20 == 0)
	        	// LPRINTF("[P2]:%d\n", end-start);
	         //LPRINTF("[RPU]: Periodic Task 2 execution time: %d\n", end-start);
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
	 const TickType_t xFrequency = 90;
	 int first = 1;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();

	         activeWaiting(10*15);

	         end = xTaskGetTickCount();
	         //LPRINTF("[RPU]: Periodic Task 3 execution time: %d\n", end-start);
	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 3 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}

void periodicTask4(void *unused_arg)
{
	 LPRINTF("[RPU]: Periodic Task 4 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 const TickType_t xFrequency = 100;
	 int first = 1;

	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();

	         activeWaiting(10*20);

	         end = xTaskGetTickCount();
	         //LPRINTF("[RPU]: Periodic Task 4 execution time: %d\n", end-start);
	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 4 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}

void periodicTask5(void *unused_arg)
{
	 LPRINTF("[RPU]: Periodic Task 5 alive...\n");
	 TickType_t xLastWakeTime;
	 TickType_t start;
	 TickType_t restart_time;
	 TickType_t end;
	 TickType_t WCET,media;
	 const TickType_t xFrequency = 300;
	 int first = 1;
	 int i=0;
	     // Initialize the xLastWakeTime variable with the current time.
	     xLastWakeTime = xTaskGetTickCount();

	     for( ;; )
	     {
	         // Wait for the next cycle.
	         vTaskDelayUntil( &xLastWakeTime, xFrequency );

	         // Perform action here.
	         restart_time = xTaskGetTickCount() - start;
	         start = xTaskGetTickCount();

	         activeWaiting(300);

	         end = xTaskGetTickCount();
	         //LPRINTF("[RPU]: Periodic Task 5 execution time: %d\n", end-start);
	         //Se il tempo di restart + l'esecuzione del task è maggiore di due volte il periodo -> DEADLINE superata
	         if(((restart_time + (end-start)) > 2*xFrequency) && first==0 )	LPRINTF("[RPU]: Periodic Task 5 DEADLINE EXCEEDED");
	         if(first==1)first = 0;
	     }
}


void aperiodicTask1(void *unused_arg)
{

	 LPRINTF("[RPU]: Aperiodic Task 1 alive t time: %d \n", pdTICKS_TO_MS(xTaskGetTickCount()));
	 uint32_t start = 0;
	 uint32_t end = 0;
	 const int N = 10;
	 int fn, fn1, fn2;
//	 uint32_t WCET = 0;
//	 uint32_t BCET = 1000;
//	 uint32_t media = 0;


//	 for (int j = 0; j < 1000 ; j++){

//	 start = xTaskGetTickCount();

	 fn1 = 1;
	 fn2 = 1;
	 fn = 0;

	 for (int i = 0; i< N; i++ ){

		 if ( i <= 1 ){
//				LPRINTF("%d iteration : %d ts = %d ms\n", i+1, 1, pdTICKS_TO_MS(xTaskGetTickCount() - start));

		 }

		 else{		
			 fn = fn1 + fn2;
//			 LPRINTF("%d iteration : %d ts = %d ms\n", i+1, fn , pdTICKS_TO_MS(xTaskGetTickCount() - start));
			 fn2 = fn1;
			 fn1 = fn;
		 }

		 activeWaiting(500);

	 }
	 
	 LPRINTF("[ATASK1] Aperiodic task 1 terminate its execution at time: %d \n", pdTICKS_TO_MS(xTaskGetTickCount()));

	 
//	 end = xTaskGetTickCount();
//	 TickType_t time = end -start;
//	 LPRINTF("\n  end - start (in ticks) = %d", time);
//	 LPRINTF("\n  end - start (in ms) = %d \n",pdTICKS_TO_MS(time));
//	 LPRINTF("\n  effective time (in ms) = %d \t (in ticks) = %d\n",pdTICKS_TO_MS(totalRuntime),totalRuntime);

//		 if (WCET < end - start){
//			 WCET = end - start;
//		 }
//
//
//			 media += end - start;
//
//
//		 if (BCET > end - start){
//				 BCET = end - start;
//			 }

//	 }
//
//	 media /= 1000;
//	 LPRINTF("\n  in ticks WCET : %d ; BCET : %d ; mean = %d", WCET, BCET,media);
//	 LPRINTF("\n  in ms WCET : %d ; BCET : %d ; mean = %d \n", pdTICKS_TO_MS(WCET) , pdTICKS_TO_MS(BCET),
//			 	 	 	 	 pdTICKS_TO_MS(media));
//	 vTaskDelete(NULL);
	 deleteATask();
}


