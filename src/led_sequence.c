#include "led_sequence.h"
#include "colors.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Move red led back and forth twice (four swipes total)
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
        .max_frames = 100,
    };
}