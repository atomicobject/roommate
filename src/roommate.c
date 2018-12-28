#include "roommate.h"
#include <sys/time.h>    /* for timeval type */
#include "led_utils.h"
#include "led_control.h"
#include "colors.h"
#include "message_buffer.h"

#define ROOMMATE_BUFFER_CAPACITY 2
#define ROOMMATE_TASK_STACK_SIZE 2048
#define ROOMMATE_TASK_PRIORITY   1

void roommate_task(void * p_context);
void set_current_time(time_t time);
void handle_updated_events(struct app_state * const p_app_state, struct calendar_events_data * const p_events);

QueueHandle_t roommate_begin(struct app_state * const p_app_state) {
    QueueHandle_t queue_handle = xQueueCreate(ROOMMATE_BUFFER_CAPACITY, sizeof(struct roommate_event));
    configASSERT( queue_handle != NULL );

    ( void ) xTaskCreate( roommate_task,             /* The function that implements the demo task. */
                          "Roommate",                /* The name to assign to the task being created. */
                          ROOMMATE_TASK_STACK_SIZE,  /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state,               /* The task parameter is not being used. */
                          ROOMMATE_TASK_PRIORITY,    /* The priority at which the task being created will run. */
                          NULL );                    /* Not storing the task's handle. */

    return queue_handle;
}

void roommate_task(void * p_context) {
    struct app_state * p_app_state = p_context;
    struct calendar_events_data current_calendar_events = {
        .num_events = 0
    };

    for(;;) {
        struct roommate_event event;
        xQueueReceive(p_app_state->roommate_queue, &event, portMAX_DELAY);
        configPRINTF(("Roommate task received an event!\r\n"));
        switch (event.type) {
            case ROOMMATE_EVENT_SET_CLOCK:
                set_current_time(event.set_clock_data.time);
                handle_updated_events(p_app_state, &current_calendar_events);
            break;
            case ROOMMATE_EVENT_UPDATE_CALENDAR_EVENTS:
                current_calendar_events = event.calendar_events_data;
                handle_updated_events(p_app_state, &current_calendar_events);
            break;
        }
    }
}

void set_current_time(time_t time) {
     configPRINTF(("Roommate task setting current time!\r\n"));
    struct timeval new_time = {
      .tv_sec = time,
      .tv_usec = 0,
    };
    settimeofday(&new_time, NULL); // NULL is for timezone, we don't need timezone since
}

struct led_data {
    time_t start;
    time_t end;
    uint32_t color;
};

struct led_control_request led_msg;
void handle_updated_events(struct app_state * const p_app_state, struct calendar_events_data * const p_events) {
    configPRINTF(("Roommate task updating %d new calendar events!\r\n", p_events->num_events));
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    configPRINTF(("Current time is:%ul\r\n", tv.tv_sec));
    struct led_data led_time_chunks[NUM_LEDS];
    const int seconds_per_chunk = 60 * 15;
    time_t seconds_remaining_in_chunk = tv.tv_sec % seconds_per_chunk;
    time_t timezone_offset = 0; //6 * 60 * 60; // TODO: Account for EST / DST
    time_t chunk_start = tv.tv_sec - timezone_offset - seconds_remaining_in_chunk;

    for(int i = 0; i < NUM_LEDS; i++) {
        led_time_chunks[i] = (struct led_data){
            .start = chunk_start,
            .end = chunk_start + seconds_per_chunk - 1
        };
        configPRINTF(("LED chunk start: %d, end: %d!\r\n", led_time_chunks[i].start, led_time_chunks[i].end));
        chunk_start += seconds_per_chunk;
    }

    bool first_event_is_ongoing = false;
    for (int led = 0; led < NUM_LEDS; led++) {
        struct led_data * p_this_led = &led_time_chunks[led];
        p_this_led->color = AO_GREEN;
        configPRINTF(("Checking LED chunk %d to %d...\r\n", p_this_led->start, p_this_led->end ));

        for (int j = 0; j < p_events->num_events; j++) {
            struct calendar_event_times this_event = p_events->events[j];
            // configPRINTF(("  Checking event %d to %d...\r\n", this_event.start, this_event.end ));
            if (this_event.start > p_this_led->end) {
                // configPRINTF(("    Event start (%d) is greater than LEDs end (%d)!\r\n", this_event.start, p_this_led->end ));
                // This event is too far in the future. No event coincides with this LED.
                break;
            }
            if (this_event.end - 1 < p_this_led->start) {  // -1 from event end because google calendar events overlap
                // This event happened before this LEDs time window. Check the next event.
                // configPRINTF(("    Event end (%d) is before LEDs start (%d)!\r\n", this_event.end, p_this_led->start ));
                continue; 
            }
            // The event must fall within this LEDs time window
            if (led == 0) {
                first_event_is_ongoing = true;
                p_this_led->color = AO_YELLOW;
            } else if (first_event_is_ongoing && j == 0) {
                p_this_led->color = AO_YELLOW;
            } else {
                p_this_led->color = AO_RED;
            }
            break;
        }

        configPRINTF(("LED[%d] brightness: %d!\r\n", led, p_this_led->color));
    }

    led_msg.type = LED_CONTROL_STEADY_STATE_REQUEST;
    for (int i = 0; i < NUM_LEDS; i++) {
        led_msg.steady_state_update_request_data.leds[i] = led_time_chunks[i].color;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        configPRINTF(("LED[%d]: %d\r\n", i, led_msg.steady_state_update_request_data.leds[i]));
    }

    MessageBufferHandle_t led_cntrl_msg_buffer = p_app_state->led_control_msg_buffer;
    xMessageBufferSend(led_cntrl_msg_buffer, &led_msg, sizeof(struct led_control_request), portMAX_DELAY);
}
