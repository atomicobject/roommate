#ifndef APP_STATE_H
#define APP_STATE_H
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "aws_mqtt_agent.h"
#include "semphr.h"
#include "event_groups.h"

#include "FreeRTOS_IP.h"

#define COUNT_OF( arr ) (sizeof(arr)/sizeof(arr[0]))


typedef struct mac_address {
    uint8_t mac [ ipMAC_ADDRESS_LENGTH_BYTES ];
} MacAddr_t;

struct app_state {
  MessageBufferHandle_t led_control_msg_buffer;
  MQTTAgentHandle_t mqtt_agent_handle;
  EventGroupHandle_t mqtt_agent_event_group;
  MessageBufferHandle_t aws_event_coordinator_buffer;
  QueueHandle_t roommate_queue;
  MacAddr_t mac_address;
};

#endif
