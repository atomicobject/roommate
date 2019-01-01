#ifndef ROOMMATE_H
#define ROOMMATE_H

#include "app_state.h"
#include "queue.h"
#include "cal_util.h"

enum roommate_event_type {
    ROOMMATE_EVENT_SET_CLOCK,
    ROOMMATE_EVENT_UPDATE_CALENDAR_EVENTS,
    ROOMMATE_EVENT_REEVALUATE_CALENDAR_EVENTS,
    ROOMMATE_EVENT_HANDLE_SHORT_BUTTON_PRESS,
};

struct clock_info {
    time_t time;
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
