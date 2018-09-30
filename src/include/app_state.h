#ifndef APP_STATE_H
#define APP_STATE_H
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "aws_mqtt_agent.h"
#include "semphr.h"
#include "event_groups.h"

struct app_state {
  MessageBufferHandle_t led_control_msg_buffer;
  MQTTAgentHandle_t mqtt_agent_handle;
  EventGroupHandle_t mqtt_agent_event_group;
  MessageBufferHandle_t aws_event_coordinator_buffer;
};

#endif
