#ifndef LED_CONTROL_H
#define LED_CONTROL_H
#include "led_control_hw.h"
#include <stdint.h>
#include "queue.h"
#include "app_state.h"

enum led_control_request_type { 
    LED_CONTROL_SEQUENCE_REQUEST, 
    LED_CONTROL_STEADY_STATE_REQUEST,
};

struct led_control_request {
    enum led_control_request_type type;
    union {
        struct led_sequence sequence_request_data;
        struct led_state steady_state_update_request_data;
    };
};

QueueHandle_t led_control_start_controlling_leds(struct app_state const * const p_app_state);

#endif