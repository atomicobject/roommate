// FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

// Application Includes
#include "led_cycle_task.h"
#include "include/led_control.h"

void led_cycle_task_run( void * pvParameters );

#define LED_CYCLE_TASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2)
#define LED_CYCLE_TASK_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority

void led_cycle_task_start(struct app_state * p_app_state) {


    configPRINTF(("Starting LED Cycle Task!\r\n") );

    ( void ) xTaskCreate( led_cycle_task_run,     /* The function that implements the demo task. */
                          "LEDTestTask",           /* The name to assign to the task being created. */
                          LED_CYCLE_TASK_STACK_SIZE,     /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state->led_control_msg_buffer,         /* The task parameter is not being used. */
                          LED_CYCLE_TASK_TASK_PRIORITY,  /* The priority at which the task being created will run. */
                          NULL );                     /* Not storing the task's handle. */
}

void led_cycle_task_run( void * pvParameters ) {
  configPRINTF(("Running LED Cycle Task!\r\n") );
  MessageBufferHandle_t msg_buffer_handle = pvParameters;
  struct led_control_msg msg;


  const TickType_t TWO_SECONDS = pdMS_TO_TICKS( 2000UL );

  for(;;) {

    vTaskDelay(TWO_SECONDS);

    configPRINTF(("Sending LED Cycle message...\r\n") );
    xMessageBufferSend(msg_buffer_handle, &msg, sizeof(struct led_control_msg), portMAX_DELAY);
    configPRINTF(("LED Cycle message sent. \r\n") );

    // memset( &msg, 0x00, sizeof( struct led_control_msg ) );
    // size_t bytes_received = xMessageBufferReceive( msg_buffer_handle,
    //                                         &msg,
    //                                         sizeof( struct led_control_msg ),
    //                                         portMAX_DELAY );

    // if (bytes_received != sizeof(struct led_control_msg)) {
    //   configPRINTF( ( "ERROR:  Unexpected message length received: Expected: %d Got: %d\r\n", sizeof(struct led_control_msg), bytes_received ) );
    //   continue;
    // }
    // configPRINTF(("Received LED Control message!\r\n") );

    // led_control_hw_test();
  }
}