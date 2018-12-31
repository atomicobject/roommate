#ifndef ROOMMATE_H
#define ROOMMATE_H

#include "app_state.h"
#include "queue.h"

#define MAX_CALENDAR_EVENTS_PER_MESSAGE 10

enum roommate_event_type {
    ROOMMATE_EVENT_SET_CLOCK,
    ROOMMATE_EVENT_UPDATE_CALENDAR_EVENTS,
    ROOMMATE_EVENT_REEVALUATE_CALENDAR_EVENTS,
};

struct clock_info {
    time_t time;
};

struct calendar_event_times {
    time_t start;
    time_t end;
};

struct calendar_events_data {
    struct calendar_event_times events[MAX_CALENDAR_EVENTS_PER_MESSAGE];
    uint32_t num_events;
};

struct roommate_event {
    enum roommate_event_type type;
    union {
        struct clock_info set_clock_data;
        struct calendar_events_data calendar_events_data;
    };
};

QueueHandle_t roommate_begin(struct app_state * const p_app_state);

#endif
