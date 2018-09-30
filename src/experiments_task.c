#include <string.h>
// FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include <sys/time.h>                   /* gettimeofday() */

// Application Includes
#include "experiments_task.h"
#include "include/led_control.h"
#include "led_sequence.h"

void experiments_task_run( void * pvParameters );

#define EXPERIMENTS_TASK_STACK_SIZE ( 900 )
#define EXPERIMENTS_TASK_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority

char task_list_buffer[512];

void experiments_task_start(struct app_state * p_app_state) {


    configPRINTF(("Starting LED Cycle Task!\r\n") );

    ( void ) xTaskCreate( experiments_task_run,                  /* The function that implements the demo task. */
                          "ExperimentsTask",                       /* The name to assign to the task being created. */
                          EXPERIMENTS_TASK_STACK_SIZE,           /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state->led_control_msg_buffer, /* The task parameter is not being used. */
                          EXPERIMENTS_TASK_TASK_PRIORITY,        /* The priority at which the task being created will run. */
                          NULL );                              /* Not storing the task's handle. */
}

void experiments_task_run( void * pvParameters ) {
  configPRINTF(("Running LED Cycle Task!\r\n") );
  MessageBufferHandle_t msg_buffer_handle = pvParameters;


  struct led_control_request msg = {
    .type = LED_CONTROL_SEQUENCE_REQUEST,
    .sequence_request_data = led_sequence_shakes_head(),
  };


  const TickType_t FIVE_SECONDS = pdMS_TO_TICKS( 5000 );

  for(;;) {
    configPRINTF(("Sending LED Cycle message...\r\n") );
    xMessageBufferSend(msg_buffer_handle, &msg, sizeof(struct led_control_request), portMAX_DELAY);
    configPRINTF(("LED Cycle message sent. \r\n") );


    memset(task_list_buffer, 0x00, sizeof(task_list_buffer));
    vTaskList(task_list_buffer);
    configPRINTF(("Task Info: len(%d)\r\n", strlen(task_list_buffer)));
    configPRINT(task_list_buffer);


    struct timeval tv;
    int gettimeofday_res;
    gettimeofday_res = gettimeofday(&tv, NULL);

    configPRINTF(("Current Time: %d\r\n", tv.tv_sec) );

    vTaskDelay(FIVE_SECONDS);
  }
}