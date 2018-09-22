// FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

// Application Includes
#include "experiments_task.h"
#include "include/led_control.h"
#include "led_sequence.h"

void experiments_task_run( void * pvParameters );

#define EXPERIMENTS_TASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2)
#define EXPERIMENTS_TASK_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority

void experiments_task_start(struct app_state * p_app_state) {


    // configPRINTF(("Starting LED Cycle Task!\r\n") );

    ( void ) xTaskCreate( experiments_task_run,                  /* The function that implements the demo task. */
                          "LEDTestTask",                       /* The name to assign to the task being created. */
                          EXPERIMENTS_TASK_STACK_SIZE,           /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state->led_control_msg_buffer, /* The task parameter is not being used. */
                          EXPERIMENTS_TASK_TASK_PRIORITY,        /* The priority at which the task being created will run. */
                          NULL );                              /* Not storing the task's handle. */
}

void experiments_task_run( void * pvParameters ) {
  // configPRINTF(("Running LED Cycle Task!\r\n") );
  MessageBufferHandle_t msg_buffer_handle = pvParameters;


  struct led_control_request msg = {
    .type = LED_CONTROL_SEQUENCE_REQUEST,
    .sequence_request_data = led_sequence_newtons_cradle(),
  };


  const TickType_t FIVE_SECONDS = pdMS_TO_TICKS( 5000 );

  for(;;) {
    // configPRINTF(("Sending LED Cycle message...\r\n") );
    xMessageBufferSend(msg_buffer_handle, &msg, sizeof(struct led_control_request), portMAX_DELAY);
    // configPRINTF(("LED Cycle message sent. \r\n") );
    vTaskDelay(FIVE_SECONDS);
  }
}