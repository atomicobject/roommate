#ifndef APP_STATE_H
#define APP_STATE_H
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "aws_mqtt_agent.h"
#include "semphr.h"


struct app_state {
  MessageBufferHandle_t led_control_msg_buffer;
  MQTTAgentHandle_t mqtt_agent_handle;
  SemaphoreHandle_t mqtt_agent_connection_semaphore;
};

#endif
