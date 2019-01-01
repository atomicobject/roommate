#ifndef CAL_UTIL_H
#define CAL_UTIL_H
#include <stdint.h>
#include "time.h"
#include "led_utils.h"

#define MAX_CALENDAR_EVENTS_PER_MESSAGE 10

struct time_range {
    time_t start;
    time_t end;
};

struct calendar_events_data {
    struct time_range events[MAX_CALENDAR_EVENTS_PER_MESSAGE];
    uint32_t num_events;
};

struct led_boundaries {
    struct time_range leds[NUM_LEDS];
};

enum led_status {
   LED_STATUS_ROOM_AVAILABLE,
   LED_STATUS_CURRENT_MEETING,
   LED_STATUS_FUTURE_MEETING, 
};

struct led_statuses {
    enum led_status leds[NUM_LEDS];
};

void cal_util_get_led_boundaries(time_t const current_timestamp, struct led_boundaries * const boundaries);

struct led_statuses cal_util_get_led_statuses(struct led_boundaries const * const boundaries,
                                              struct calendar_events_data * const p_events);

int cal_util_get_color_for_status(enum led_status status);

#endif
