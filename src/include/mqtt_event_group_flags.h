#ifndef MQTT_EVENT_GROUP_FLAGS
#define MQTT_EVENT_GROUP_FLAGS
#include "event_groups.h"

#define MQTT_EVENT_AGENT_CONNECTED          (EventBits_t)(1 << 0)
#define MQTT_EVENT_AGENT_DISCONNECTED       (EventBits_t)(1 << 1)
#define MQTT_EVENT_DATA_READY               (EventBits_t)(1 << 2)

#endif