#include "roommate.h"
#include "led_utils.h"
#include "led_control.h"
#include "aws_event_coordinator.h"
#include "mqtt_event_group_flags.h"
#include "cal_util.h"
#include "time_utils.h"
#include "led_sequence.h"

#define ROOMMATE_BUFFER_CAPACITY 2
#define ROOMMATE_TASK_STACK_SIZE 2200
#define ROOMMATE_TASK_PRIORITY   1

void roommate_task(void * p_context);
void minute_timer_callback(TimerHandle_t timer);
void reset_minute_alarm(TimerHandle_t minute_timer, time_t time);
void handle_updated_events(struct app_state * const p_app_state, struct calendar_events_data * const p_events);
void extend_or_reserve_room(struct app_state * const p_app_state, struct calendar_events_data * const p_events);

const TickType_t ONE_MINUTE_TICKS = pdMS_TO_MIN_TICKS(60000);

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

    TimerHandle_t minute_timer = xTimerCreate("minute_timer",
                 1, // Initial period doesn't matter. Timer is created in dormant state.
                 pdTRUE,
                 p_app_state->roommate_event_queue, // Give the timer access to the queue so it can dispatch events
                 minute_timer_callback);

    for(;;) {
        struct roommate_event event;
        xQueueReceive(p_app_state->roommate_event_queue, &event, portMAX_DELAY);
        configPRINTF(("Roommate task received an event!\r\n"));
        switch (event.type) {
            case ROOMMATE_EVENT_SET_CLOCK:
                time_utils_set_current_time(event.set_clock_data.time);
                reset_minute_alarm(minute_timer, event.set_clock_data.time);
                handle_updated_events(p_app_state, &current_calendar_events);
            break;
            case ROOMMATE_EVENT_UPDATE_CALENDAR_EVENTS:
                current_calendar_events = event.calendar_events_data;
                handle_updated_events(p_app_state, &current_calendar_events);
            break;
            case ROOMMATE_EVENT_REEVALUATE_CALENDAR_EVENTS:
                handle_updated_events(p_app_state, &current_calendar_events);
            break;
            case ROOMMATE_EVENT_HANDLE_SHORT_BUTTON_PRESS:
                extend_or_reserve_room(p_app_state, &current_calendar_events);
            break;
        }
    }
}

void reset_minute_alarm(TimerHandle_t minute_timer, time_t time) {
    time_t seconds_until_next_minute = 60 - (time % 60);
    TickType_t ticks_until_next_minute = pdMS_TO_TICKS(seconds_until_next_minute * 1000);
    xTimerChangePeriod(minute_timer, ticks_until_next_minute, portMAX_DELAY);
    xTimerStart(minute_timer, portMAX_DELAY);
}

void minute_timer_callback(TimerHandle_t timer) {
    configPRINTF(("Minute timer expired. Refresh LEDs!\r\n"));
    TickType_t period = xTimerGetPeriod(timer);
    if(period != ONE_MINUTE_TICKS) {
        xTimerChangePeriod(timer, ONE_MINUTE_TICKS, portMAX_DELAY);
    }

    QueueHandle_t roommate_queue = pvTimerGetTimerID(timer);
    struct roommate_event event = {
        .type = ROOMMATE_EVENT_REEVALUATE_CALENDAR_EVENTS
    };
    xQueueSend(roommate_queue, &event, portMAX_DELAY);
}

bool get_time_range_to_book(struct calendar_events_data * const p_events, 
                            struct time_range * p_time_range_out,
                            uint8_t * p_led_index_for_range) {
    struct led_boundaries led_time_chunks;
    time_t current_time = time_utils_get_current_time();
    cal_util_get_led_boundaries(current_time, &led_time_chunks);
    struct led_statuses led_statuses = cal_util_get_led_statuses(&led_time_chunks, p_events);

    if (led_statuses.leds[0] == LED_STATUS_ROOM_AVAILABLE) {
        // Reserve room for the first boundary
        *p_time_range_out = led_time_chunks.leds[0];
        return true;
    } else {
        // Find the first window that is not current meeting
        for (int i = 0; i < NUM_LEDS; i++){
            enum led_status status = led_statuses.leds[i];
            if (status != LED_STATUS_CURRENT_MEETING) {
                if (status == LED_STATUS_ROOM_AVAILABLE) {
                    // We can extend the meeting to this time window!
                    *p_time_range_out = led_time_chunks.leds[i];
                    *p_led_index_for_range = i;
                    return true;
                }
                // Can't book. There's a future meeting.
                return false;
            }
        }
    }
    return false;
}

void begin_pending_led_animation(struct app_state * const p_app_state, struct calendar_events_data * const p_events, uint8_t led_index) {
    struct led_boundaries led_time_chunks;
    time_t current_time = time_utils_get_current_time();
    cal_util_get_led_boundaries(current_time, &led_time_chunks);
    struct led_statuses led_statuses = cal_util_get_led_statuses(&led_time_chunks, p_events);
    struct led_control_request led_msg;
    led_msg.type = LED_CONTROL_SEQUENCE_REQUEST;
    struct led_state led_state;
    for (int i = 0; i < NUM_LEDS; i++) {
        led_state.leds[i] = cal_util_get_color_for_status(led_statuses.leds[i]);
    }
    led_msg.sequence_request_data = led_sequence_pulse_led(led_state, led_index, NULL);
    QueueHandle_t led_ctrl_queue = p_app_state->led_control_queue;
    xQueueSend(led_ctrl_queue, &led_msg, portMAX_DELAY);


}

void extend_or_reserve_room(struct app_state * const p_app_state, struct calendar_events_data * const p_events) {
    // At this point we cannot extend or reserve a room. Send the shakes head gesture.
    struct time_range range_to_book;
    uint8_t pending_led_index = 0;
    if (get_time_range_to_book(p_events, &range_to_book, &pending_led_index)) {
        begin_pending_led_animation(p_app_state, p_events, pending_led_index);
        struct aws_event aws_msg = {
            .type = AWS_EVENT_REQUEST_ROOM_HOLD,
        };
        aws_msg.room_hold_data.start = range_to_book.start;
        aws_msg.room_hold_data.finish = range_to_book.end;
        configPRINTF(("Sending message to aws_event_coordinator to reserve room\r\n"));
        xQueueSend(p_app_state->aws_event_coordinator_queue, &aws_msg, portMAX_DELAY);
        xEventGroupSetBits(p_app_state->mqtt_agent_event_group, MQTT_EVENT_DATA_READY);
    } else {
        // Can't book the room. Shake head
        struct led_control_request led_msg;
        led_msg.type = LED_CONTROL_SEQUENCE_REQUEST;
        led_msg.sequence_request_data = led_sequence_shakes_head();
        QueueHandle_t led_ctrl_queue = p_app_state->led_control_queue;
        xQueueSend(led_ctrl_queue, &led_msg, portMAX_DELAY);
    }
}

void handle_updated_events(struct app_state * const p_app_state, struct calendar_events_data * const p_events) {
    struct led_boundaries led_time_chunks;
    time_t current_time = time_utils_get_current_time();
    cal_util_get_led_boundaries(current_time, &led_time_chunks);
    struct led_statuses led_statuses = cal_util_get_led_statuses(&led_time_chunks, p_events);
    struct led_control_request led_msg;
    led_msg.type = LED_CONTROL_STEADY_STATE_REQUEST;
    for (int i = 0; i < NUM_LEDS; i++) {
        led_msg.steady_state_update_request_data.leds[i] = cal_util_get_color_for_status(led_statuses.leds[i]);
        // configPRINTF(("LED[%d]: %d\r\n", i, led_msg.steady_state_update_request_data.leds[i]));
    }
    QueueHandle_t led_ctrl_queue = p_app_state->led_control_queue;
    xQueueSend(led_ctrl_queue, &led_msg, portMAX_DELAY);
}
