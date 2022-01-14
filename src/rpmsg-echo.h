#ifndef RPMSG_ECHO_H
#define RPMSG_ECHO_H

#define RPMSG_SERVICE_NAME         "rpmsg-openamp-demo-channel"
//#define RPMSG_SERVICE_NAME2        "rpmsg-openamp-demo-second"


#include "queue-RTRA.h"
#include "pending-request-queue.h"
#include "event_groups.h"
#include "timers.h"


//event definition

#define EVENT_QUEUE 	0x01
#define EVENT_NEW_TASK 	0x02
#define EVENT_STOP     	0x04
#define EVENT_SUSPEND 	0x08


//task handler for sporadic server's implementation
static int runningIndex;
static TaskHandle_t runningTask;
static uint16_t suspendedTask = 0;
static TaskHandle_t ssTask;


static EventGroupHandle_t eventManager;
static queue_PR *pendingRequest;
static RTRAqueue *pendingRA;
static uint32_t request_code;

//parameter of SS
#define CS_MAX 5
#define TS 	   50
static TickType_t CS = CS_MAX ;
static TickType_t CS_old = 0;
static TickType_t TA = 0;


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
 * set --> when CS < WCET
   reset --> when suspend task
 * */
static uint8_t controlling = 0;
/*startTime
 * set --> in switched in when the next running task is aperiodic task
 * reset --> not necessary
 * */
static TickType_t startTime = 0;

/* runTime
 * sum --> when the outgoing task is aperiodic task and controlling = 1
 *  reset --> when reset controlling
 * */
static TickType_t runtime = 0;

/* totalRunTime
 * */
static TickType_t totalRuntime = 0;





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
