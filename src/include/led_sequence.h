#ifndef LED_SEQUENCE_H
#define LED_SEQUENCE_H
#include "led_utils.h"

struct led_sequence led_sequence_shakes_head(void);
struct led_sequence led_sequence_newtons_cradle_blue(continue_sequence_func should_continue_func);
struct led_sequence led_sequence_newtons_cradle_green(continue_sequence_func should_continue_func);
struct led_sequence led_sequence_pulse_led(struct led_state initial_state, uint8_t led_index, 
                                           continue_sequence_func should_continue_func);

#endif