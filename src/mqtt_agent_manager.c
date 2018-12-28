#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "aws_mqtt_agent.h"
#include "aws_clientcredential.h"
#include "aws_wifi.h"
#include <string.h>

#include "app_state.h"
#include "mqtt_agent_manager.h"
#include "mqtt_event_group_flags.h"

#define MQTT_AGENT_MANAGER_TASK_STACK_SIZE ( 2048 )
#define MQTT_AGENT_MANAGER_TASK_PRIORITY   ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority
#define MQTT_AGENT_MANAGER_CONNECT_FLAGS   ( mqttagentREQUIRE_TLS )

#define MQTT_AGENT_MANAGER_TLS_NEGOTIATION_TIMEOUT   pdMS_TO_TICKS( 12000 )

void maintain_mqtt_agent_connection(void * task_param);
BaseType_t mqtt_agent_event_callback_handler( void * p_user_data, const MQTTAgentCallbackParams_t * const p_callback_params );
BaseType_t createClientAndConnectToBroker( struct app_state * p_app_state );

void mqtt_agent_manager_begin(struct app_state * p_app_state) {
    configPRINTF(("Starting MQTT Agent Manager Task...\r\n"));
    ( void ) xTaskCreate( maintain_mqtt_agent_connection,      /* The function that implements the demo task. */
                          "MQTTAgentManagerTask",              /* The name to assign to the task being created. */
                          MQTT_AGENT_MANAGER_TASK_STACK_SIZE,  /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state,                         /* The task parameter is not being used. */
                          MQTT_AGENT_MANAGER_TASK_PRIORITY,    /* The priority at which the task being created will run. */
                          NULL );                              /* Not storing the task's handle. */
    /* Create the MQTT client object and connect it to the MQTT broker. */
}

void maintain_mqtt_agent_connection(void * task_param) {
    struct app_state * p_app_state = task_param;
    p_app_state->mqtt_agent_event_group = xEventGroupCreate();
    configPRINTF(("MQTT Agent Manager Task started. Created event group %d\r\n", p_app_state->mqtt_agent_event_group));
    for(;;) {
        if (WIFI_IsConnected() == pdFALSE) {
            configPRINTF(("MQTT Agent Manager - No WiFi. Delaying for 5 seconds\r\n"));
            vTaskDelay(pdMS_TO_TICKS(5000));

        } else if (pdPASS == createClientAndConnectToBroker(p_app_state)) {
            xEventGroupSetBits(p_app_state->mqtt_agent_event_group, MQTT_EVENT_AGENT_CONNECTED);
            // Just hang out here forever or until the agent connection is broken.
            configPRINTF(("MQTT Agent Manager - Waiting for agent disconnect\r\n"));
            xEventGroupWaitBits(p_app_state->mqtt_agent_event_group, MQTT_EVENT_AGENT_DISCONNECTED, pdTRUE, pdTRUE, portMAX_DELAY);
            xEventGroupClearBits(p_app_state->mqtt_agent_event_group, MQTT_EVENT_AGENT_CONNECTED);
            configPRINTF(("MQTT Agent Manager - Detected agent disconnect\r\n"));
        } else {
            configPRINTF(("Delaying for 1 second before attempting MQTT connect again\r\n"));
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

#define MAKE_CLIENT_ID(CLIENT_ID_NAME, P_APP_STATE ) \
    uint8_t* _mac = P_APP_STATE->mac_address.mac; \
    uint8_t clientId[ 6*3+20]; \
    sprintf((char*)clientId, "%02X:%02X:%02X:%02X:%02X:%02X", _mac[0],_mac[1],_mac[2],_mac[3],_mac[4],_mac[5]);

BaseType_t createClientAndConnectToBroker( struct app_state * p_app_state ) {
    MQTTAgentReturnCode_t last_mqtt_action_result = eMQTTAgentFailure;
    BaseType_t xReturn = pdFAIL;

    MAKE_CLIENT_ID(clientId, p_app_state);

    MQTTAgentConnectParams_t xConnectParameters =
    {
        clientcredentialMQTT_BROKER_ENDPOINT, /* The URL of the MQTT broker to connect to. */
        MQTT_AGENT_MANAGER_CONNECT_FLAGS,     /* Connection flags. */
        pdFALSE,                              /* Deprecated. */
        clientcredentialMQTT_BROKER_PORT,     /* Port number on which the MQTT broker is listening. Can be overridden by ALPN connection flag. */
        clientId,                            /* Client Identifier of the MQTT client. It should be unique per broker. */
        0,                                    /* The length of the client Id, filled in later as not const. */
        pdFALSE,                              /* Deprecated. */
        p_app_state,                          /* User data supplied to the callback. Can be NULL. */
        mqtt_agent_event_callback_handler,    /* Callback used to report various events. Can be NULL. */
        NULL,                                 /* Certificate used for secure connection. Can be NULL. */
        0                                     /* Size of certificate used for secure connection. */
    };

    /* Check this function has not already been executed. */
    if ( p_app_state->mqtt_agent_handle == NULL ) {
        configPRINTF( ( "MQTT Agent Manager creating agent with clientId %s...\r\n",clientId ) );
        last_mqtt_action_result = MQTT_AGENT_Create( &p_app_state->mqtt_agent_handle);
    } else {
        last_mqtt_action_result = eMQTTAgentSuccess;
        configPRINTF( ( "MQTT Agent Manager - Agent is already created.\r\n" ) );
    }

    /* The MQTT client object must be created before it can be used.  The
     * maximum number of MQTT client objects that can exist simultaneously
     * is set by mqttconfigMAX_BROKERS. */

    if( last_mqtt_action_result == eMQTTAgentSuccess )
    {
        MAKE_CLIENT_ID(clientId, p_app_state);

        /* Fill in the MQTTAgentConnectParams_t member that is not const,
         * and therefore could not be set in the initializer (where
         * xConnectParameters is declared in this function). */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) clientId );

        /* Connect to the broker. */
        configPRINTF( ( "MQTT Agent Manager - MQTT attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        last_mqtt_action_result = MQTT_AGENT_Connect( p_app_state->mqtt_agent_handle,
                                                      &xConnectParameters,
                                                      MQTT_AGENT_MANAGER_TLS_NEGOTIATION_TIMEOUT);

        if( last_mqtt_action_result != eMQTTAgentSuccess )
        {
            /* Could not connect, so delete the MQTT client. */
            ( void ) MQTT_AGENT_Delete( p_app_state->mqtt_agent_handle );
            configPRINTF( ( "ERROR: MQTT Agent Manager - MQTT failed to connect.\r\n" ) );
        }
        else
        {
            configPRINTF( ( "MQTT Agent Manger - Agent connected!\r\n" ) );
            xReturn = pdPASS;
        }
    }

    return xReturn;
}

BaseType_t mqtt_agent_event_callback_handler( void * p_user_data, const MQTTAgentCallbackParams_t * const p_callback_params ) {
    if (p_callback_params->xMQTTEvent == eMQTTAgentDisconnect) {
        configPRINTF( ( "MQTT Agent Manager - MQTT callback handler detected disconnect %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        // The agent was disconnected, handle this event.
        // Send on a semaphore here so that MQTT connection monitor can reconnect
        struct app_state * p_app_state = p_user_data;
        // Don't think this callback actually runs in an interrupt context to this next call is ok.
        xEventGroupSetBits(p_app_state->mqtt_agent_event_group, MQTT_EVENT_AGENT_DISCONNECTED);
    }
    return pdFALSE; // This means that we are not responsible for freeing the buffer memory. See aws_mqtt_agent.h:100
}
