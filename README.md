# firmware-RPU-ZCU104

NOTE: to make the project work it is necessary (for calculate the execution time of running aperiodic task) to define the functions of the switch context in freeRTOS.H: 
      #define traceTASK_SWITCHED_OUT()  --->  #define traceTASK_SWITCHED_OUT(arg) controlSwitchingOut(arg) 
      #define traceTASK_SWITCHED_IN()  --->  #define traceTASK_SWITCHED_IN(arg) controlSwitchingIN(arg)
      
   and add a pxCurrentTCB as parameter when the function vTaskSwitchContext() (in tasks.c) called it:
          traceTASK_SWITCHED_OUT();  --->  traceTASK_SWITCHED_OUT(pxCurrentTCB);
          traceTASK_SWITCHED_OUT();  --->  traceTASK_SWITCHED_OUT(pxCurrentTCB); 
          
         
