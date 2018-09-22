#include "led_sequence.h"
#include "colors.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

struct led_frame shakes_head(uint32_t frame_num, uint32_t elapsed_time_ms) {
    const uint32_t ms_per_swipe = 200;
    const uint32_t frame_duration_ms = ms_per_swipe / NUM_LEDS;

    uint8_t const led_states[] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        6, 5, 4, 3, 2, 1, 0,
        1, 2, 3, 4, 5, 6, 
        5, 4, 3, 2, 1,
        2, 3, 4, 5, 
    };

    size_t const num_states = sizeof(led_states);

    struct led_frame frame = {
        .duration_ms = frame_duration_ms,
        .is_last_frame = false,
    };

    memset(&frame.new_state.leds, 0, sizeof(struct led_state));

    uint8_t lit_led = led_states[frame_num];
    frame.new_state.leds[lit_led] = RED;
    if (frame_num >= num_states - 1) {
        frame.is_last_frame = true;
    }

    return frame;
}

struct led_sequence led_sequence_shakes_head(void) {
    return (struct led_sequence) {
        .executor = &shakes_head,
        .max_frames = 100, // Higher than needed (incase we adjust the sequence)
    };
}

struct led_frame newtons_cradle(uint32_t frame_num, uint32_t elapsed_time_ms) {
    struct led_frame frame;
    memset(&frame.new_state.leds, 0, sizeof(struct led_state));
    const uint8_t num_leds = NUM_LEDS - 1;
    const uint8_t total_passes = 10;
    const uint16_t frame_duration = 50;
    bool moving_right = (frame_num / num_leds) % 2 == 0;
    uint8_t lit_led = moving_right ? (frame_num % num_leds) : num_leds - (frame_num % num_leds) - 1;
    if (moving_right && lit_led >= 3) lit_led += 1;
    if (!moving_right && lit_led <= 4 && lit_led != 0) lit_led -= 1;
    frame.new_state.leds[lit_led] = BLUE;
    frame.new_state.leds[4] = BLUE;  
    frame.new_state.leds[3] = BLUE;  
    frame.duration_ms = lit_led == 7 ? frame_duration * 2 : frame_duration;
    frame.is_last_frame = frame_num >= total_passes * num_leds;
    return frame;
}

struct led_sequence led_sequence_newtons_cradle(void) {
    return (struct led_sequence) {
        .executor = &newtons_cradle,
        .max_frames = 200, // Higher than needed (incase we adjust the sequence)
    };
}