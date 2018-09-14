// Standard Includes
#include "stdint.h"
#include "string.h"
#include "stdio.h"

// FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "task.h"

// App Includes
#include "led_control.h"
#include "led_utils.h"

#define LED_CONTROL_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2 )
#define LED_CONTROL_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority
#define LED_CONTROL_BUFFER_CAPACITY ((size_t)2)
const TickType_t FIFTY_MICROSECONDS = pdMS_TO_TICKS(10) / 20;

led_control_start_controlling_leds(void) {

    MessageBufferHandle_t msg_buffer_handle = xMessageBufferCreate( (sizeof(struct led_control_request) + sizeof(size_t)) * LED_CONTROL_BUFFER_CAPACITY );
    configASSERT( msg_buffer_handle != NULL );


    ( void ) xTaskCreate( led_control,                /* The function that implements the demo task. */
                          "LEDControlTask",           /* The name to assign to the task being created. */
                          LED_CONTROL_STACK_SIZE,     /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          msg_buffer_handle,          /* The task parameter is not being used. */
                          LED_CONTROL_TASK_PRIORITY,  /* The priority at which the task being created will run. */
                          NULL );                     /* Not storing the task's handle. */


    struct led_state all_off_state = LED_STATE_ALL_OFF();
    struct led_control_request initial_msg = {
        .type = LED_CONTROL_STEADY_STATE_REQUEST,
        .steady_state_update_request_data = all_off_state,
    };
    xMessageBufferSend(msg_buffer_handle, &initial_msg, sizeof(initial_msg), portMAX_DELAY);

    return msg_buffer_handle;
}

void led_control(void * params) {
    MessageBufferHandle_t msg_buffer_handle = params;
    struct led_control_request msg;
    struct led_state current_steady_state = LED_STATE_ALL_OFF();

    for(;;) {
        memset( &msg, 0x00, sizeof( struct led_control_request ) );
        size_t bytes_received = xMessageBufferReceive( msg_buffer_handle,
                                                &msg,
                                                sizeof( struct led_control_request ),
                                                portMAX_DELAY );

        if (bytes_received != sizeof(struct led_control_request)) {
            configPRINTF( ( "ERROR:  Unexpected message length received: Expected: %d Got: %d\r\n", sizeof(struct led_control_request), bytes_received ) );
            continue;
        } else {
            configPRINTF(("Received LED Control message!\r\n") );
        }

        switch (msg.type) {
            case LED_CONTROL_STEADY_STATE_REQUEST:
                current_steady_state = msg.steady_state_update_request_data;

                // Update the LEDs

            break;

            case LED_CONTROL_SEQUENCE_REQUEST:
                uint32_t frame_num = 0;
                uint32_t elapsed_time_ms = 0;
                do  {
                    struct led_frame new_frame = msg.sequence_request_data.executor(frame_num, elapsed_time_ms);

                    // Update the LEDs 

                    // Delay for the frame duration
                    vTaskDelay(FIFTY_MICROSECONDS); // Datasheet says this is required after each transaction
                    vTaskDelay(pdMS_TO_TICKS(new_frame.duration_ms));

                    // Increment frame and elapsed time
                    frame_num++;
                    elapsed_time_ms += new_frame.duration_ms;

                } while (frame_num < msg.sequence_request_data.max_frames);
            break;
        }

    }
}
