// Standard Includes
#include "stdint.h"
#include "string.h"
#include "stdio.h"

// FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

// App includes
#include "led_control.h"
#include "led_control_hw.h"

#define LED_CONTROL_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2 )
#define LED_CONTROL_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority
#define LED_CONTROL_BUFFER_CAPACITY ((size_t)2)
 

// Drive SPI at 4MHZ
//https://productize.be/driving-ws2812-programmable-rgb-leds-using-hardware-spi/

struct led_control_state {
  MessageBufferHandle_t msg_buffer_handle;
};


void prvStartLEDControlTask( void * pvParameters );

MessageBufferHandle_t startLEDControlTask(void) {

    MessageBufferHandle_t msg_buffer_handle = xMessageBufferCreate( (sizeof(struct led_control_msg) + sizeof(size_t)) * LED_CONTROL_BUFFER_CAPACITY );
    configASSERT( msg_buffer_handle != NULL );


    ( void ) xTaskCreate( prvStartLEDControlTask,     /* The function that implements the demo task. */
                          "LEDControlTask",           /* The name to assign to the task being created. */
                          LED_CONTROL_STACK_SIZE,     /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          msg_buffer_handle,         /* The task parameter is not being used. */
                          LED_CONTROL_TASK_PRIORITY,  /* The priority at which the task being created will run. */
                          NULL );                     /* Not storing the task's handle. */


    struct led_control_msg initial_msg = {};
    xMessageBufferSend(msg_buffer_handle, &initial_msg, sizeof(initial_msg), portMAX_DELAY);

    return msg_buffer_handle;
}

void prvStartLEDControlTask( void * pvParameters ) {
  MessageBufferHandle_t msg_buffer_handle = pvParameters;
  struct led_control_msg msg;

  led_control_hw_init();

  for(;;) {
    memset( &msg, 0x00, sizeof( struct led_control_msg ) );

    configPRINTF(("Waiting for LED Control message!\r\n") );
    size_t bytes_received = xMessageBufferReceive( msg_buffer_handle,
                                            &msg,
                                            sizeof( struct led_control_msg ),
                                            portMAX_DELAY );

    if (bytes_received != sizeof(struct led_control_msg)) {
      configPRINTF( ( "ERROR:  Unexpected message length received: Expected: %d Got: %d\r\n", sizeof(struct led_control_msg), bytes_received ) );
      continue;
    } else {
      configPRINTF(("Received LED Control message!\r\n") );
    }

    led_control_hw_test();
  }
}