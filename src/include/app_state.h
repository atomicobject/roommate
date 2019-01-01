#ifndef APP_STATE_H
#define APP_STATE_H
#include "FreeRTOS.h"
#include "aws_mqtt_agent.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_IP.h"

#define COUNT_OF( arr ) (sizeof(arr)/sizeof(arr[0]))


#define MAKE_CLIENT_ID(CLIENT_ID_NAME, P_APP_STATE ) \
    uint8_t* _mac = P_APP_STATE->mac_address.mac; \
    uint8_t clientId[ 6*3+20]; \
    sprintf((char*)clientId, "%02X:%02X:%02X:%02X:%02X:%02X", _mac[0],_mac[1],_mac[2],_mac[3],_mac[4],_mac[5]);

typedef struct mac_address {
    uint8_t mac [ ipMAC_ADDRESS_LENGTH_BYTES ];
} MacAddr_t;

struct app_state {
  QueueHandle_t led_control_queue;
  MQTTAgentHandle_t mqtt_agent_handle;
  EventGroupHandle_t mqtt_agent_event_group;
  QueueHandle_t aws_event_coordinator_queue;
  QueueHandle_t roommate_event_queue;
  MacAddr_t mac_address;
};

#endif
