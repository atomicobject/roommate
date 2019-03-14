#include "led_sequence.h"
#include "colors.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

struct led_frame shakes_head(uint32_t frame_num, uint32_t elapsed_time_ms) {
    (void)elapsed_time_ms;
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

struct led_frame newtons_cradle(uint32_t frame_num, uint32_t elapsed_time_ms, uint32_t color) {
    (void)elapsed_time_ms;
    struct led_frame frame;
    memset(&frame.new_state.leds, 0, sizeof(struct led_state));
    const uint8_t total_passes = 5;
    uint8_t seq[] = { 0x11, 0x12, 0x14, 0x18, 0x28, 0x48, 0x88, 0x88, 0x48, 0x28, 0x18, 0x14, 0x12, 0x11 };
    const uint8_t num_states = 14;
    uint8_t active_state = seq[frame_num % num_states];
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        frame.new_state.leds[i] = active_state & (0x01 << i) ? color : LED_OFF;
    }
    frame.duration_ms = 50;
    frame.is_last_frame = frame_num >= total_passes * num_states;
    return frame;
}

struct led_frame newtons_cradle_blue(uint32_t frame_num, uint32_t elapsed_time_ms) {
    return newtons_cradle(frame_num, elapsed_time_ms, BLUE);
}

struct led_sequence led_sequence_newtons_cradle_blue(continue_sequence_func should_continue_func) {
    return (struct led_sequence) {
        .executor = &newtons_cradle_blue,
        .max_frames = 200, // Higher than needed (incase we adjust the sequence)
        .should_continue = should_continue_func
    };
}

struct led_frame newtons_cradle_green(uint32_t frame_num, uint32_t elapsed_time_ms) {
    return newtons_cradle(frame_num, elapsed_time_ms, GREEN);
}

struct led_sequence led_sequence_newtons_cradle_green(continue_sequence_func should_continue_func) {
    return (struct led_sequence) {
        .executor = &newtons_cradle_green,
        .max_frames = 200, // Higher than needed (incase we adjust the sequence)
        .should_continue = should_continue_func
    };
}

struct pulse_led_state {
    struct led_state initial_state;
    uint8_t led_index;
} pulse_led_state;

struct led_frame pulse_led(uint32_t frame_num, uint32_t elapsed_time_ms) {
    struct led_frame frame = {
        .new_state = pulse_led_state.initial_state,
        .duration_ms = 50,
        .is_last_frame = false,
    };
    frame.new_state.leds[pulse_led_state.led_index] = GREEN + (frame_num * 10);
    return frame;
}

struct led_sequence led_sequence_pulse_led(struct led_state initial_state, uint8_t led_index, 
                                           continue_sequence_func should_continue_func) {
    
    pulse_led_state.initial_state = initial_state;
    pulse_led_state.led_index = led_index;

    return (struct led_sequence) {
        .executor = &pulse_led,
        .max_frames = 200, // Higher than needed (incase we adjust the sequence)
        .should_continue = should_continue_func
    };
}