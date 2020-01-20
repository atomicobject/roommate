#ifndef AWS_EVENT_COORDINATOR_H
#define AWS_EVENT_COORDINATOR_H
#include "calendar_event.h"
#include "roommate.h"

// FreeRTOS Includes
#include "queue.h"
#include "app_state.h"

enum aws_event_type {
    AWS_EVENT_CALENDAR_DATA_RECEIVED,
    AWS_EVENT_REQUEST_CALENDAR_DATA,
    AWS_EVENT_REQUEST_ROOM_HOLD,
};


struct calendar_data {
    timestamp_t current_time; 
    struct calendar_event events[MAX_CALENDAR_EVENTS_PER_MESSAGE];
    uint8_t num_events;
};

struct room_hold_data {
    time_t start;
    time_t finish;
};

struct aws_event {
    enum aws_event_type type;
    union {
        struct calendar_data calendar_data;
        struct room_hold_data room_hold_data;
    };
};

QueueHandle_t aws_event_coordinator_start_coordinating(struct app_state * const p_app_state);

#endif