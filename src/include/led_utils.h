#ifndef LED_UTILS_H
#define LED_UTILS_H
#include <stdint.h>
#include <stdbool.h>

#define NUM_LEDS 8

struct led_state {
    uint32_t leds[NUM_LEDS];
};

struct led_frame {
    struct led_state new_state;
    uint16_t duration_ms;
    bool is_last_frame;
};

// typedef void (*led_sequence_state_init_func)(struct led_sequence_state * p_state);
typedef struct led_frame (*led_sequence_executor_func)(uint32_t frame_num, uint32_t elapsed_time_ms);

struct led_sequence {
    led_sequence_executor_func executor;       // Called in a loop to get the next LED state (frame) until return value is null
    // led_sequence_state_init_func initializer;  // Called to get initial state for sequence 
    uint32_t max_frames;
};


const uint8_t gamma8[];

#define RGBB(R, G, B, S) \
  ( \
    ((uint32_t)gamma8[(uint8_t)((uint32_t)R * S)] << 16) | \
    ((uint32_t)gamma8[(uint8_t)((uint32_t)G * S)] <<  8) | \
    ((uint32_t)gamma8[(uint8_t)((uint32_t)B * S)] <<  0)   \
  )

#define LED_STATE_ALL_OFF() (struct led_state) { \
  .leds = {0, 0, 0, 0, 0, 0, 0, 0}               \
}


void led_utils_test(void);

#endif