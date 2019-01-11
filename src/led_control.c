// Standard Includes
#include "stdint.h"
#include "string.h"
#include "stdio.h"

// FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"

// App Includes
#include "led_control.h"
#include "led_control_hw.h"
#include "led_utils.h"

#define LED_CONTROL_STACK_SIZE ( 2048 )
#define LED_CONTROL_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority
#define LED_CONTROL_BUFFER_CAPACITY ((size_t)2)
const TickType_t FIFTY_MICROSECONDS = pdMS_TO_TICKS(10) / 20;

void control_leds(void * params);
void set_leds_and_delay_ms(struct led_state new_state, uint32_t delay_ms);

QueueHandle_t led_control_start_controlling_leds(struct app_state const * const p_app_state) {
    led_control_hw_init();

    QueueHandle_t queue_handle = xQueueCreate(LED_CONTROL_BUFFER_CAPACITY, sizeof(struct led_control_request));
    configASSERT( queue_handle != NULL );

    ( void ) xTaskCreate( control_leds,                /* The function that implements the demo task. */
                          "LEDControlTask",           /* The name to assign to the task being created. */
                          LED_CONTROL_STACK_SIZE,     /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state,          /* The task parameter is not being used. */
                          LED_CONTROL_TASK_PRIORITY,  /* The priority at which the task being created will run. */
                          NULL );                     /* Not storing the task's handle. */

    return queue_handle;
}

void control_leds(void * params) {
    struct app_state * p_app_state = params;
    struct led_control_request msg;
    struct led_state current_steady_state = LED_STATE_ALL_OFF();

    for(;;) {
        memset(&msg, 0x00, sizeof(struct led_control_request));
        BaseType_t success = xQueueReceive(p_app_state->led_control_queue,
                                           &msg,
                                           portMAX_DELAY);

        if (success != pdTRUE) {
            configPRINTF(( "ERROR: Tried to dequeue led_control messasge but failed"));
            continue;
        } else {
            configPRINTF(("Received LED Control message!\r\n") );
        }

        switch (msg.type) {
            case LED_CONTROL_STEADY_STATE_REQUEST:
                // Update the steady state and the LEDs
                current_steady_state = msg.steady_state_update_request_data;
                configPRINTF(("Setting LEDs to new steady state!\r\n"));
                // for (int i = 0; i < NUM_LEDS; i++) {
                //     configPRINTF(("LED[%d]: %d\r\n", i, current_steady_state.leds[i]));
                // }

                set_leds_and_delay_ms(current_steady_state, 0);
            break;

            case LED_CONTROL_SEQUENCE_REQUEST:
            {
                uint32_t frame_num = 0;
                uint32_t elapsed_time_ms = 0;
                for (;;) {
                    do  {
                        struct led_frame new_frame = msg.sequence_request_data.executor(frame_num, elapsed_time_ms);
                        // Update the LEDs 
                        set_leds_and_delay_ms(new_frame.new_state, new_frame.duration_ms);

                        if (new_frame.is_last_frame) { break; }

                        // Increment frame and elapsed time
                        frame_num++;
                        elapsed_time_ms += new_frame.duration_ms;

                    } while (frame_num < msg.sequence_request_data.max_frames);

                    // Run the sequence again unless should_continue is false
                    if (msg.sequence_request_data.should_continue == NULL ||
                        msg.sequence_request_data.should_continue(p_app_state) == false) {
                            break;
                        }
                }

                // Finally, after the sequence is over, return to the steady state
                set_leds_and_delay_ms(current_steady_state, 0);
            }
            break;

            default:
                configPRINTF(("ERROR: Unrecognized LED Control Message Type received!\r\n") );
        }
    }
}

void set_leds_and_delay_ms(struct led_state new_state, uint32_t delay_ms) {
    led_control_hw_write_leds(new_state);
    // Datasheet says we need to sleep for at least 50 microseconds after writing to LEDs
    TickType_t delay = delay_ms > 0 ? pdMS_TO_TICKS(delay_ms) : FIFTY_MICROSECONDS;
    vTaskDelay(delay); 
}
