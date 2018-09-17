#ifndef LED_CONTROL_HW_H
#define LED_CONTROL_HW_H
#include "led_utils.h"

void led_control_hw_init(void);
void led_control_hw_write_leds(struct led_state new_state);


#endif