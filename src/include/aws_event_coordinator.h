#ifndef AWS_EVENT_COORDINATOR_H
#define AWS_EVENT_COORDINATOR_H
#include "calendar_event.h"

// FreeRTOS Includes
#include "message_buffer.h"
#include "app_state.h"

enum aws_event_type {
    AWS_EVENT_CALENDAR_DATA_RECEIVED,
    AWS_EVENT_REQUEST_CALENDAR_DATA,
    AWS_EVENT_REQUEST_ROOM_HOLD,
};

#define MAX_CALENDAR_EVENTS_PER_MESSAGE 10

struct calendar_data {
    timestamp_t current_time; 
    struct calendar_event events[MAX_CALENDAR_EVENTS_PER_MESSAGE];
    uint8_t num_events;
};

struct aws_event {
    enum aws_event_type type;
    union {
        struct calendar_data calendar_data;
    };
};

MessageBufferHandle_t aws_event_coordinator_start_coordinating(struct app_state * const p_app_state);

#endif