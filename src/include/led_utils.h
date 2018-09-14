#ifndef LED_UTILS_H
#define LED_UTILS_H
#include "led_control.h"

#define LED_STATE_ALL_OFF() (struct led_state) { \
  .leds = {0, 0, 0, 0, 0, 0, 0, 0}               \
}


void led_utils_test(void);

#endif