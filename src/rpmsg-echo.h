#ifndef RPMSG_ECHO_H
#define RPMSG_ECHO_H

#define RPMSG_SERVICE_NAME         "rpmsg-openamp-demo-channel"


#include "queue-RTRA.h"
#include "pending-request-queue.h"
#include "event_groups.h"
#include "timers.h"


//event definition

#define EVENT_QUEUE 	0x01
#define EVENT_NEW_TASK 	0x02
#define EVENT_STOP     	0x04
#define EVENT_SUSPEND 	0x08


//parameter of SS
#define CS_MAX 25*45
#define TS 	   250*45
static TickType_t CS = CS_MAX ;
static TickType_t CS_old = 0;
static TickType_t TA = 0;



static TaskHandle_t ssTask;				//task handler for sporadic server's implementation
static TaskHandle_t runningTask;		//task handler to running aperiod task
static int runningIndex;				//index to running aperiod task
static uint16_t suspendedTask = 0;		//flag to check if running aperiod task has been suspended



static EventGroupHandle_t eventManager;	//main reference to eventGroup
static queue_PR *pendingRequest;		//reference to queue in wich to store pending requests
static RTRAqueue *pendingRA;			//reference to queue in wich to store pending RA
static uint32_t request_code;			//variable to store request from APU



//Timer's ID and timer's task definition
#define N_MAX_TIMER 1000
#define TIMER_CS_ID 0
static uint16_t TIMER_TS_ID = 1;
static uint16_t RA_TOT = 0;
static TimerHandle_t timerCSOLD;
static TaskHandle_t TCStask;
static TaskHandle_t TTStask;

/**********paramenter for calculation of runtime execution ***************************/
/* controlling
 * set --> when start the execution of task
   reset --> when suspend or finish task's work
 * */
static uint8_t controlling = 0;

/*startTime
 * set --> in switched in when the next running task is aperiodic task
 * reset --> updated to current time in switched out to avoid multiple assignments
 * */
static TickType_t startTime = 0;

/* runTime: task execution time in a single slot of capacity
 * sum --> when the outgoing task is aperiodic task and controlling = 1
 *  reset --> when reset controlling
 * */
static TickType_t runtime = 0;

/* totalRunTime: total execution time of running task
 * sum --> increment with runtime when a slot of capacity is finished
 * reset --> when the task is finished
 * */
static TickType_t totalRuntime = 0;
static int c = 0;




/*****tasks ss manager protypes ******/

void sporadicServer();
void controlRunningTask();
void checkWCET(TickType_t sub_wcet, int index, int suspend);
void startTasks(int index,TickType_t wcet,int suspend);
void timerCallBack( TimerHandle_t timerHandler);
void timerTS(void * pvParameters);
void timerCS(void * pvParameters);

// service function prototypes

void controlSwitchingIN(TaskHandle_t * nextTcb);
void controlSwitchingOut(TaskHandle_t * currentTcb);
void deleteATask();


#endif /* RPMSG_ECHO_H */
