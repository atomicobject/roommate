#include "FreeRTOS.h"
#include "task.h"
#include "aws_mqtt_agent.h"
#include "aws_clientcredential.h"

#include "app_state.h"
#include "mqtt_agent_manager.h"

#define MQTT_AGENT_MANAGER_TASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2 )
#define MQTT_AGENT_MANAGER_TASK_PRIORITY   ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority
#define MQTT_AGENT_MANAGER_CONNECT_FLAGS   ( mqttagentREQUIRE_TLS )

// TODO: Figure out what to use for this identifier? What does "unique per broker" mean? Should this be something
// like "Roommate-Klein"?
#define CLIENT_ID                           ( ( const uint8_t * ) "CalendarEventHandler" )
#define MQTT_AGENT_MANAGER_TLS_NEGOTIATION_TIMEOUT   pdMS_TO_TICKS( 12000 )

void calendar_event_handler_begin(struct app_state * p_app_state) {
    ( void ) xTaskCreate( maintain_mqtt_agent_connection,      /* The function that implements the demo task. */
                          "MQTT_Agent_Manager",                /* The name to assign to the task being created. */
                          MQTT_AGENT_MANAGER_TASK_STACK_SIZE,  /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          NULL,                                /* The task parameter is not being used. */
                          MQTT_AGENT_MANAGER_TASK_PRIORITY,    /* The priority at which the task being created will run. */
                          NULL );                              /* Not storing the task's handle. */
    /* Create the MQTT client object and connect it to the MQTT broker. */
}

void maintain_mqtt_agent_connection(struct app_state * p_app_state) {
    p_app_state->mqtt_agent_connection_semaphore = xSemaphoreCreateBinary();
    for(;;) {
        if (eMQTTAgentSuccess == createClientAndConnectToBroker(p_app_state)) {
            // Just hang out here forever or until the agent connection is broken.
            xSemaphoreTake(p_app_state->mqtt_agent_connection_semaphore, portMAX_DELAY);
        } else {
            configPRINTF("Delaying for 1 second before attempting MQTT connect again");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

static BaseType_t createClientAndConnectToBroker( struct app_state * p_app_state ) {
    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn = pdFAIL;
    MQTTAgentConnectParams_t xConnectParameters =
    {
        clientcredentialMQTT_BROKER_ENDPOINT, /* The URL of the MQTT broker to connect to. */
        MQTT_AGENT_MANAGER_CONNECT_FLAGS,     /* Connection flags. */
        pdFALSE,                              /* Deprecated. */
        clientcredentialMQTT_BROKER_PORT,     /* Port number on which the MQTT broker is listening. Can be overridden by ALPN connection flag. */
        CLIENT_ID,                            /* Client Identifier of the MQTT client. It should be unique per broker. */
        0,                                    /* The length of the client Id, filled in later as not const. */
        pdFALSE,                              /* Deprecated. */
        p_app_state,                          /* User data supplied to the callback. Can be NULL. */
        mqtt_agent_event_callback_handler,    /* Callback used to report various events. Can be NULL. */
        NULL,                                 /* Certificate used for secure connection. Can be NULL. */
        0                                     /* Size of certificate used for secure connection. */
    };

    /* Check this function has not already been executed. */
    configASSERT( p_app_state->mqtt_agent_handle == NULL );

    /* The MQTT client object must be created before it can be used.  The
     * maximum number of MQTT client objects that can exist simultaneously
     * is set by mqttconfigMAX_BROKERS. */
    xReturned = MQTT_AGENT_Create( &p_app_state->mqtt_agent_handle);

    if( xReturned == eMQTTAgentSuccess )
    {
        /* Fill in the MQTTAgentConnectParams_t member that is not const,
         * and therefore could not be set in the initializer (where
         * xConnectParameters is declared in this function). */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) CLIENT_ID );

        /* Connect to the broker. */
        configPRINTF( ( "MQTT Agent Manager - MQTT attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        xReturned = MQTT_AGENT_Connect( p_app_state->mqtt_agent_handle,
                                        &xConnectParameters,
                                        MQTT_AGENT_MANAGER_TLS_NEGOTIATION_TIMEOUT);

        if( xReturned != eMQTTAgentSuccess )
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
        // The agent was disconnected, handle this event.
        // Send on a semaphore here so that MQTT connection monitor can reconnect
        struct app_state * p_app_state = p_user_data;
        // Don't think this callback actually runs in an interrupt context to this next call is ok.
        xSemaphoreGive(p_app_state->mqtt_agent_connection_semaphore); // Signal that client was disconnected...
    }
    return pdFALSE; // This means that we are not responsible for freeing the buffer memory. See aws_mqtt_agent.h:100
}

// MQTTBool_t prvMQTTCallback( void * pvUserData,
//                                    const MQTTPublishData_t * const pxPublishParameters )

// }

// static BaseType_t prvSubscribe( void )
// {
//     MQTTAgentReturnCode_t xReturned;
//     BaseType_t xReturn;
//     MQTTAgentSubscribeParams_t xSubscribeParams;

//     /* Setup subscribe parameters to subscribe to echoTOPIC_NAME topic. */
//     xSubscribeParams.pucTopic = echoTOPIC_NAME;
//     xSubscribeParams.pvPublishCallbackContext = NULL;
//     xSubscribeParams.pxPublishCallback = prvMQTTCallback;
//     xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) echoTOPIC_NAME );
//     xSubscribeParams.xQoS = eMQTTQoS1;

//     /* Subscribe to the topic. */
//     xReturned = MQTT_AGENT_Subscribe( xMQTTHandle,
//                                       &xSubscribeParams,
//                                       democonfigMQTT_TIMEOUT );

//     if( xReturned == eMQTTAgentSuccess )
//     {
//         configPRINTF( ( "MQTT Echo demo subscribed to %s\r\n", echoTOPIC_NAME ) );
//         xReturn = pdPASS;
//     }
//     else
//     {
//         configPRINTF( ( "ERROR:  MQTT Echo demo could not subscribe to %s\r\n", echoTOPIC_NAME ) );
//         xReturn = pdFAIL;
//     }

//     return xReturn;
// }