#ifndef APP_STATE_H
#define APP_STATE_H
#include "FreeRTOS.h"
#include "message_buffer.h"

struct app_state {
  MessageBufferHandle_t led_control_msg_buffer;
};

#endif
