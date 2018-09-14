#ifndef LED_CONTROL_H
#define LED_CONTROL_H
#include "led_control_hw.h"
#include <stdint.h>

struct led_state {
    uint32_t leds[NUM_LEDS];
};

struct led_frame {
    struct led_state new_state;
    uint8_t duration_ms
};

enum led_control_request_type { 
    LED_CONTROL_SEQUENCE_REQUEST, 
    LED_CONTROL_STEADY_STATE_REQUEST 
};

// typedef void (*led_sequence_state_init_func)(struct led_sequence_state * p_state);
typedef struct led_frame (*led_sequence_executor_func)(uint32_t frame_num, uint32_t elapsed_time_ms);

struct led_sequence {
    led_sequence_executor_func executor;       // Called in a loop to get the next LED state (frame) until return value is null
    // led_sequence_state_init_func initializer;  // Called to get initial state for sequence 
    uint32_t max_frames;
};

struct led_control_request {
    enum led_control_request_type type;
    union {
        struct led_sequence sequence_request_data;
        struct led_state steady_state_update_request_data;
    };
};



#endif