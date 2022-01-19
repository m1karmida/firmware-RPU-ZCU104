

//Xilinx
#include "xil_printf.h"
#include "xtime_l.h"
//OpenAMP-Libmetal
#include <openamp/open_amp.h>
#include <metal/alloc.h>
#include "rsc_table.h"
#include "platform_info.h"
#include "rpmsg-echo.h"
//freeRTOS
#include "FreeRTOS.h"
#include "task.h"

#include "RPU_task.h"

//Commands and Tasks definitions
#define SHUTDOWN_MSG	0xEF56A55A
#define LOAD_MSG		0XFF56A55A

#define MAX_DATA_LENGHT 492

//Print end errors
#define LPRINTF(format, ...) xil_printf(format, ##__VA_ARGS__)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

//Time definitions
#define DELAY_1_SECOND		1000UL

//Static variable
static struct rpmsg_endpoint lept;		//Endpoint
static int shutdown_req = 0;			//Request to shutdown
static uint32_t load = (CS_MAX*100)/TS ;				//Current RPU load

//static int debug_var = 123; Da decommentare per il DEBUG

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

//Print the log on the screen
static int printscreen = 1;

//Payload struct received from remote processor
struct _payload {
	uint32_t command;				//32 bit (4 bytes)
	char data[MAX_DATA_LENGHT];		//dati del payload  (492 bytes)
};

//Shared Variables
static struct _payload shared_variable;		//A shared variable for each channel is needed
static struct rpmsg_endpoint shared_ept;	//A Shared endpoint for each channel is needed

//TaskManager handle
static TaskHandle_t comm_task;

/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint callbacks
 *-----------------------------------------------------------------------------*/
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
				 uint32_t src, void *priv)
{

	struct _payload *payload = (struct _payload *)data;
	shared_variable = (*payload);
	shared_ept = *ept;
	return RPMSG_SUCCESS;
}


static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	LPRINTF("[RPU]: unexpected Remote endpoint destroy\n");
	shutdown_req = 1;
}


/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int TaskManager(struct rpmsg_device *rdev, void *priv)
{
	int ret,j;
	int task_load;					//load of the requested Task
	BaseType_t state;				//Starting Task state
	uint32_t ans;					//Answer to delete request
	uint32_t addr = 0;				//Auto-increment Address for channels

	/*Variables for TaskManager frequency*/
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;

	/*Shared variable initialization*/
	shared_variable.command = 0x00000000;

	/* Initialize RPMSG framework */
	if(printscreen) LPRINTF("[RPU]: TASK MANAGER-> Try to create rpmsg endpoint.\n");
	ret = rpmsg_create_ept(&lept, rdev, RPMSG_SERVICE_NAME,
				0, RPMSG_ADDR_ANY, rpmsg_endpoint_cb,
				rpmsg_service_unbind);
	if (ret) {
		LPERROR("[RPU]: TASK MANAGER-> Failed to create endpoint.\n");
		return -1;
	}
	if(printscreen) LPRINTF("[RPU]: TASK MANAGER-> Successfully created rpmsg endpoint.\n");

	/* Initialize the xLastWakeTime variable with the current time.*/
	xLastWakeTime = xTaskGetTickCount();


	/* Initialization ss task*/
	  xTaskCreate(sporadicServer, "SS Task", 256, NULL, 4, &ssTask);


	for( ;; )
	{

		// Wait for the next cycle.
		vTaskDelayUntil( &xLastWakeTime, xFrequency);

		/* Perform action here.*/

		// Wait for messages from
		platform_poll(priv);

		if(shared_variable.command == 0x00000000){
			if(printscreen) LPRINTF("\n[RPU]: No Requests.\n");
		}

		/* On reception of a shutdown we signal the application to terminate */
		else if (shared_variable.command == SHUTDOWN_MSG) {
			if(printscreen) LPRINTF("\n[RPU]: shutdown message is received.\n");

			/*Shutdown the tasks*/
			j=0;
			while(j<RPU_N_TASK){
				if(Task_vector[j].running == 1){
					vTaskDelete(*Task_vector[j].Handler);						//Delete the Task
					load = load - Task_vector[j].load;							//Remove task load from RPU load
					Task_vector[j].running = 0;									//Task j is not more running
					if(printscreen) LPRINTF("[RPU]: %s deleted\n", Task_vector[j].name);
				}
				j++;
			}

			ans = 0x00000000;  											//All done!
			rpmsg_send(&shared_ept, &ans, sizeof(uint32_t));

			break;
		}

		else if(shutdown_req == 1){
			LPRINTF("\n[RPU]: Unexpected shutdown.\n");
			break;
		}

		/*Load Request*/
		else if(shared_variable.command == LOAD_MSG){
			if(printscreen) LPRINTF("\n[RPU]: Load request is received.\n");
			if(printscreen) LPRINTF("[RPU]: Current Load: %ld%%\n", load);
			if(printscreen) LPRINTF("[RPU]: Sending data...\n");
			rpmsg_send(&shared_ept, &load, sizeof(unsigned long));
		}


		/*Offloading task request*/
		else if (shared_variable.command < 0x0001000){
			if(printscreen) LPRINTF("\n[RPU]: Offload task request is received.\n");
			if(printscreen) LPRINTF("[RPU]: Received data: %s \n",shared_variable.data);	//Debug

			j=0;
			while(Task_vector[j].code != shared_variable.command && j<RPU_N_TASK){
				j++;
			}
			/*Task request doesn't match!*/
			if(j == RPU_N_TASK){
				if(printscreen) LPRINTF("[RPU]: The requested Task doesn't exist!\n");
			}
			/*Same Periodic Task request: only one instance per periodic task */
			else if(Task_vector[j].running == 1 && j <= RPU_N_P_TASK){
				if(printscreen) LPRINTF("[RPU]: Task already running!\n");
			}
			/*Task request is correct*/
			else{

				addr = addr + 1;
			//control aperiodic / periodic
				if (shared_variable.command <= RPU_N_P_TASK){
				   //Periodic task: add load to current load and create task
					task_load = Task_vector[j].load;
					load = load + task_load;

					state = xTaskCreate(Task_vector[j].function , ( const char * ) Task_vector[j].name,
							configMINIMAL_STACK_SIZE, NULL, Task_vector[j].priority , Task_vector[j].Handler);
					if (state != pdPASS) {
						LPERROR("[RPU]: cannot create task\n");
					}
					else{
						Task_vector[j].running = 1;
						if(printscreen) LPRINTF("[RPU]: %s started. (address = 0x%08x)\n",Task_vector[j].name ,addr);

						if(printscreen) LPRINTF("[RPU]: Sending address data...\n");
						rpmsg_send(&shared_ept, &addr, sizeof(uint32_t));
					}

				}	else {
						//aperiodic task: send event to ss task
						request_code = shared_variable.command;
						if(printscreen) LPRINTF("[RPU]: Sending address data...\n");
						rpmsg_send(&shared_ept, &addr, sizeof(uint32_t));
						xEventGroupSetBits(eventManager,EVENT_NEW_TASK);
				}


			}
		}

		/*Delete task request*/
		else{
			if(printscreen) LPRINTF("\n[RPU]: Stop task request is received.\n");

			j=0;
			while(Task_vector[j].stop_code != shared_variable.command && j<RPU_N_TASK){
				j++;
			}

			/*Task request doesn't match!*/
			if(j == RPU_N_TASK){
				if(printscreen) LPRINTF("[RPU]: The requested Task doesn't exist!\n");
				ans = 0x00000001;											//Error in deleting the Task
			}
			/*Task request is correct*/
			else{
				vTaskDelete(*Task_vector[j].Handler);						//Delete the Task
				load = load - Task_vector[j].load;							//Remove task load from RPU load
				Task_vector[j].running = 0;									//Task j is not more running
				ans = 0x00000000;  											//All done!
				if(printscreen) LPRINTF("[RPU]: %s deleted\n", Task_vector[j].name);
			}

			if(printscreen) LPRINTF("[RPU]: Sending reply...\n");
			rpmsg_send(&shared_ept, &ans, sizeof(uint32_t));
		}
		//Reinitialize shared_variable command
		shared_variable.command = 0x00000000;
	}

	rpmsg_destroy_ept(&lept);

	return 0;
}


/*-----------------------------------------------------------------------------*
 *  Processing Task
 *-----------------------------------------------------------------------------*/
static void processing(void *unused_arg)
{
	void *platform;
	struct rpmsg_device *rpdev;
	const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );


	LPRINTF("[RPU]: Starting application...\n");
	/* Initialize platform */
	if (platform_init(NULL, NULL, &platform)) {
		LPERROR("[RPU]: Failed to initialize platform.\n");
	} else {
		rpdev = platform_create_rpmsg_vdev(platform, 0,
										VIRTIO_DEV_SLAVE,
										NULL, NULL);
		if (!rpdev){
			LPERROR("[RPU]: Failed to create rpmsg virtio device.\n");
		} else {
			TaskManager(rpdev, platform);
			platform_release_rpmsg_vdev(rpdev);
		}
	}
	while (1){
		vTaskDelay( x1second );
		LPRINTF(".");
	}

	LPRINTF("[RPU]: Stopping application...\n");
	platform_cleanup(platform);	//why commented?

	/* Terminate this task */
	vTaskDelete(NULL);
}




/*-----------------------------------------------------------------------------*
 *  Sporadic server's function
 *-----------------------------------------------------------------------------*/

/******************scheduler function *************************/

void controlSwitchingIN(TaskHandle_t * nextTcb){
	if (runningTask == nextTcb && controlling ){
		startTime = xTaskGetTickCount();
	}
}


void controlSwitchingOut (TaskHandle_t * currentTcb){

	if (runningTask == currentTcb && controlling ){
		runtime += xTaskGetTickCount() - startTime ;
		startTime = xTaskGetTickCount();
	} else if (!controlling)
		runtime = 0;
}
/************************************************************/

/**********************************************************
 * NB: for stopping any aperiodic task, it needs to call this function
 * *******************************************************/
void deleteATask(){
	runningTask = NULL;
	controlSwitchingOut(NULL);
	xEventGroupSetBits(eventManager, EVENT_STOP);
	vTaskDelete(NULL);
}
/***********************************************************/

void sporadicServer(){
	//initialization
		  pendingRA = init_queue();		//queue for timer TS for recharging CS
		  pendingRequest = init_queue_PR(); //queue for pending request task
		  eventManager = xEventGroupCreate();	//eventGroup for communication between tasks
		LPRINTF("[SS] sporadic server running \n");
		EventBits_t waitingEvents, uxBits, control;

		while(1){

					waitingEvents = EVENT_NEW_TASK | EVENT_QUEUE | EVENT_SUSPEND | EVENT_STOP;
					uxBits = xEventGroupWaitBits(eventManager, waitingEvents, pdFALSE,pdFALSE ,portMAX_DELAY);

					control = uxBits & EVENT_NEW_TASK;
					if (control == EVENT_NEW_TASK){
						 LPRINTF("[SS] new request \n");
						xEventGroupClearBits(eventManager, EVENT_NEW_TASK);
						int i = 0;
						while(i<RPU_N_TASK && Task_vector[i].code != request_code)
							i++;

						if (i ==RPU_N_TASK){
							LPRINTF("[SS] task not existing \n");

						} else if ( runningTask == NULL  && queuePRIsEmpty(pendingRequest)){
							runningIndex = i;
							checkWCET(0,i,0);
						} else{
							LPRINTF("[SS] resource occupied, pushing task in queue. \n");
							push_queuePR(pendingRequest, request_code);
						}


					}

					control = uxBits & EVENT_QUEUE;
					if (control == EVENT_QUEUE && runningTask == NULL){
						LPRINTF("[SS] pop task from queue. \n");

						xEventGroupClearBits(eventManager, EVENT_QUEUE);
						int taskCode = pop_queuePR(pendingRequest);
						int i = 0;
						while(i<RPU_N_TASK && Task_vector[i].code != taskCode)
							i++;
						if (i ==RPU_N_TASK){
							LPRINTF("[SS]task not exisisting \n");
						} else{
							runningIndex = i;
							checkWCET(0, i,0);
						}
					}

					control = uxBits & EVENT_STOP;
					if (control == EVENT_STOP){
						totalRuntime+=runtime;
						CS -= runtime;
						RA_TOT+=runtime;
						controlling = 0;
						totalRuntime = 0;
						xEventGroupClearBits(eventManager, EVENT_STOP);
						if(CS > 0 && !queuePRIsEmpty(pendingRequest)){

								LPRINTF("[SS] pop task from queue after stop.\n");
								int taskCode = pop_queuePR(pendingRequest);
								int i = 0;
								while(i<RPU_N_TASK && Task_vector[i].code != taskCode)
									i++;
								if (i ==RPU_N_TASK){
									LPRINTF("[SS]task not exisisting \n");
								} else{
									runningIndex = i;
									checkWCET(0, i,0);
								}
						} else{
							push_queue(pendingRA, RA_TOT);
							RA_TOT = 0;
							if (queuePRIsEmpty(pendingRequest)){
									LPRINTF("[SS] no pending request. \n");
							}
							else
								LPRINTF("[SS] waiting for update CS. \n");


						}
				}

				control = uxBits & EVENT_SUSPEND;
				if (control == EVENT_SUSPEND){
					
					xEventGroupClearBits(eventManager, EVENT_SUSPEND);
					checkWCET(totalRuntime,runningIndex,1 );

				}
		}
}

void checkWCET(TickType_t sub_wcet, int index,int suspend){

		int remainingExTime = Task_vector[index].load - sub_wcet;

	    if (CS >=remainingExTime){

	    // capacity greater than wcet: can run all the task's work
	    	startTasks(index,remainingExTime,suspend);

	    } else if (CS > 0 && CS < remainingExTime){
	   // capacity less than wcet: not enough to execute all the task
	    	CS_old = CS;
	    	//control if is a consecutive request after stop
	    	if (RA_TOT==0){
	    		//not consecutive request: create new istance of timer
	    		timerCSOLD = xTimerCreate("Timer CS OLD", CS_old, pdFALSE, (void *)TIMER_CS_ID, timerCallBack);
	    		if(xTimerStart(timerCSOLD,3) == pdFAIL){
	    			LPRINTF("[SS] ERROR: timer not started \n");
	    		} else{
	    			startTasks(index,CS_old,suspend);

	    		}
	    	} else{
    			//consecutive request: timer already active, only change period
	    		if(xTimerChangePeriod(timerCSOLD,CS_old,3) == pdFAIL){
	    			LPRINTF("[SS] ERROR: timer not started \n");
	    		} else
	    			startTasks(index,CS_old,suspend);

	    	}

	    } else{
	    	LPRINTF("[SS] waiting for update CS. \n");
	    	push_queuePR(pendingRequest,request_code);

	    }


}


void startTasks(int index,TickType_t wcet, int suspend ){

	controlling = 1;

 //control if it is a new request or resume request
	if (!suspend){
		xTaskCreate(Task_vector[index].function, Task_vector[index].name, 256, NULL,
						Task_vector[index].priority, Task_vector[index].Handler);
		runningTask = *(Task_vector[index].Handler);
	} else{
		LPRINTF("[SS] Resume at t: %d \n", xTaskGetTickCount());
		vTaskResume(runningTask);
	}
	//controlling if it is a not consecutive request: only in this case need to start a timer TS
	if (RA_TOT==0){
		TickType_t time;
		if (TA != 0){
			time = TS - (xTaskGetTickCount() - TA);

		}else
			time = TS;

		TimerHandle_t timer = xTimerCreate("Timer RT",  time, pdFALSE, (void *)TIMER_TS_ID ,timerCallBack);

		TIMER_TS_ID++;
		if (TIMER_TS_ID % N_MAX_TIMER == 0)
			TIMER_TS_ID = 1;

		if(xTimerStart(timer,3) == pdFAIL){
			LPRINTF("error, timer command queue full\n");
		}

	}

}

/*********************************************************/



/***************** timer's callbacks *********************/
void timerCallBack( TimerHandle_t timerHandler){
	switch((int)pvTimerGetTimerID(timerHandler)){

		case TIMER_CS_ID:
			xTaskCreate(timerCS, "TASK TCS_old", 256, NULL, configMAX_PRIORITIES-1,&TCStask);
			break;

		default:
			xTaskCreate(timerTS, "TASK T_TS", 256, NULL, configMAX_PRIORITIES-2,&TTStask);
			break;

	}

}


void timerTS(void * pvParameters){

	TickType_t RA = pop_queue(pendingRA);
	CS += RA;
	int HigherPriorityTaskEx = 0;

//calculate Ta only if there is at least one higher priority task running and pending request

	for (int i = 0; i < RPU_N_TASK && HigherPriorityTaskEx == 0 ; i++){

		if (*(Task_vector[i].Handler)!= NULL && eTaskGetState(*(Task_vector[i].Handler)) == eReady
				&& Task_vector[i].priority >  uxTaskPriorityGet(ssTask) && (runningTask!=NULL || !queuePRIsEmpty(pendingRequest)))
			HigherPriorityTaskEx = 1;

	}

	if (HigherPriorityTaskEx)
		TA = xTaskGetTickCount();
	else
		TA = 0;

//decide wich request needs to be made at ss task
	if (suspendedTask){
		suspendedTask = 0;
		xEventGroupSetBits(eventManager, EVENT_SUSPEND);
	}
	else if (runningTask == NULL && !queuePRIsEmpty(pendingRequest)){
		xEventGroupSetBits(eventManager, EVENT_QUEUE);
	}
	if (RA > 0)
		LPRINTF("[TIMERTS] recharged %d ticks at t %d. CS = %d\n",RA,xTaskGetTickCount(),CS );

	vTaskDelete(NULL);
}


void timerCS(void* pvParameters){
	//control if running task has finished before expire of timer
	if (runningTask!= NULL){

		long remainingExTime = (long)(CS_old - runtime);
		//control remaining execution time
		if (remainingExTime > 0){
			//positive: there have been preemption from higher priority task, so CS has not been totally consumed
			if (xTimerChangePeriod(timerCSOLD,remainingExTime,5) == pdFAIL){

				LPRINTF("[TIMERCSOLD] error recharging timer\n");

			}
		}
		else if (remainingExTime==0){
			//capacity totally consumed
	    	CS -= runtime;
	    	//check if the capacity has been updated in the meantime (timer TS expired)
	    	if (CS > 0){
	    		//updated: continue execution of running task with new remaining capacity
	    		CS_old = CS;
	    		if (xTimerChangePeriod(timerCSOLD,CS_old,5) == pdFAIL){

	    						LPRINTF("[TIMERCSOLD] error recharging timer\n");

	    					}
	    	} else {
	    		//not updated: suspend task until the timer TS expires
				totalRuntime += runtime;
				LPRINTF("[TIMERCSOLD] TE ts: %d \n", xTaskGetTickCount());
				suspendedTask = 1;
				push_queue(pendingRA, runtime + RA_TOT);
				RA_TOT = 0;
				controlling = 0;
				vTaskSuspend(runningTask);

	    	}
		} else
			LPRINTF("[TIMERCSOLD] ERROR cs overflow. runtime = %d  cs = %d \n",runtime,CS_old);

	}

	vTaskDelete(NULL);
}



/*********************************************************/


/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(void)
{
	BaseType_t stat;

	/* Create the tasks */
	stat = xTaskCreate(processing, ( const char * ) "HW2",
				1024, NULL, 2, &comm_task);
	LPRINTF("[RPU]: vTaskStartScheduler...\n");
	if (stat != pdPASS) {
		LPERROR("[RPU]: cannot create task\n");
	} else {
		/* Start running FreeRTOS tasks */
		vTaskStartScheduler();
	}

	/* Will not get here, unless a call is made to vTaskEndScheduler() */
	while (1) ;

	/* suppress compilation warnings*/
	return 0;
}
