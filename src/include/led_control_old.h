#ifndef LED_CONTROL_OLD_H
#define LED_CONTROL_OLD_H
#include "message_buffer.h"
#include <stdint.h>

struct led_control_msg {
  uint8_t desired_led_brightnesses[8];
  uint32_t desired_led_colors[8];
};

MessageBufferHandle_t startLEDControlTask(void);

#endif