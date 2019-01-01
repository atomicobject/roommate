#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>                   /* gettimeofday() */
#include "time.h"

// Application Includes
#include "app_state.h"
#include "mqtt_event_group_flags.h"
#include "aws_event_coordinator.h"
#include "calendar_event.h"
#include "jsmn.h"

#define AWS_EVENT_COORDINATOR_STACK_SIZE ( 2304 )
#define AWS_EVENT_COORDINATOR_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 ) // IDLE task is lowest priority
#define AWS_EVENT_COORDINATOR_CAPACITY ((size_t)3) // TODO: Think about how many we actually need here...

#define MQTT_SUBSCRIBE_TIMEOUT                   pdMS_TO_TICKS( 300 )

char outgoing_mqtt_msg_buffer[128];

void coordinate_events(void * p_params);
static MQTTBool_t handle_received_event_for_subscription( void * p_context, const MQTTPublishData_t * const p_publish_parameters );

QueueHandle_t aws_event_coordinator_start_coordinating(struct app_state * const p_app_state) {
    QueueHandle_t queue_handle = xQueueCreate(AWS_EVENT_COORDINATOR_CAPACITY, sizeof(struct aws_event));
    configASSERT( queue_handle != NULL );

    ( void ) xTaskCreate( coordinate_events,                    /* The function that implements the demo task. */
                          "AWS Event Coord",                    /* The name to assign to the task being created. */
                          AWS_EVENT_COORDINATOR_STACK_SIZE,     /* The size, in WORDS (not bytes), of the stack to allocate for the task being created. */
                          p_app_state,                          /* The task parameter is not being used. */
                          AWS_EVENT_COORDINATOR_TASK_PRIORITY,  /* The priority at which the task being created will run. */
                          NULL );                               /* Not storing the task's handle. */

    return queue_handle;
}


void coordinate_events(void * p_params) {
    struct app_state * const p_app_state = p_params;

    uint8_t topic_name[128];
    MAKE_CLIENT_ID( clientId, p_app_state);
    sprintf((char*)topic_name,"calendar-updates/for-board/%s", clientId);

    configPRINTF(("Event Coordinator is starting up\r\n"));
    for (;;) {
        configASSERT((p_app_state->mqtt_agent_event_group != NULL));
        // Wait for the MQTT Agent to be connected
        configPRINTF(("Waiting for bits on group %d\r\n", p_app_state->mqtt_agent_event_group));
        EventBits_t bitsSet = xEventGroupWaitBits( p_app_state->mqtt_agent_event_group, //const EventGroupHandle_t xEventGroup, 
                                                   MQTT_EVENT_AGENT_CONNECTED, //const EventBits_t uxBitsToWaitFor, 
                                                   pdTRUE, // const BaseType_t xClearOnExit, 
                                                   pdTRUE, //const BaseType_t xWaitForAllBits, 
                                                   portMAX_DELAY); //TickType_t xTicksToWait )


        // Subscribe to MQTT events using the connected agent
        MQTTAgentSubscribeParams_t subscribe_params;
        subscribe_params.pucTopic = topic_name;
        subscribe_params.pvPublishCallbackContext = p_app_state; // Pass app_state pointer to the event callback
        subscribe_params.pxPublishCallback = handle_received_event_for_subscription;
        subscribe_params.usTopicLength = ( uint16_t ) strlen( ( const char * ) topic_name);
        subscribe_params.xQoS = eMQTTQoS1; // TODO: Is this our preferred QoS

        configPRINTF(("Subscribing to MQTT topic: %s...\r\n", subscribe_params.pucTopic));
        MQTTAgentReturnCode_t subscribe_result = MQTT_AGENT_Subscribe( p_app_state->mqtt_agent_handle,
                                                                       &subscribe_params,
                                                                       MQTT_SUBSCRIBE_TIMEOUT );
        if (subscribe_result != eMQTTAgentSuccess) {
            configPRINTF(("Subscription attempt FAILED!\r\n"));
            // Start over and wait for agent to be online
            continue;
        }
        configPRINTF(("Subscription attempt succeded!\r\n"));

        // We're successfully subscribed. Begin waiting for and handling events
        for(;;) {

            configPRINTF(("Event Coordinator waiting for agent disconencted or data ready\r\n"));
            EventBits_t bitsSet = xEventGroupWaitBits( p_app_state->mqtt_agent_event_group, //const EventGroupHandle_t xEventGroup, 
                                                    MQTT_EVENT_AGENT_DISCONNECTED | 
                                                    MQTT_EVENT_DATA_READY, //const EventBits_t uxBitsToWaitFor, 
                                                    pdTRUE, // const BaseType_t xClearOnExit, 
                                                    pdFALSE, //const BaseType_t xWaitForAllBits, 
                                                    portMAX_DELAY); //TickType_t xTicksToWait )
            if (bitsSet & MQTT_EVENT_DATA_READY) {
                configPRINTF(("Event Coordinator got an event. Get it out of the queue...\r\n"));
                struct aws_event rx_event;
                BaseType_t event_received = xQueueReceive(p_app_state->aws_event_coordinator_queue, &rx_event, 100);
                if (event_received != pdTRUE) {
                    configPRINTF(("ERROR - Event Received bit was set but no event was received. How happen?\r\n"));
                    continue;
                }

                switch (rx_event.type) {
                    case AWS_EVENT_CALENDAR_DATA_RECEIVED:
                    {
                        // Update the RTC with the timestamp in the message
                        struct tm t;
                        struct roommate_event clock_update_event = {
                             .type = ROOMMATE_EVENT_SET_CLOCK,
                        };
                        strptime(rx_event.calendar_data.current_time.bytes, "%Y-%m-%dT%H:%M:%SZ", &t);
                        clock_update_event.set_clock_data.time = mktime(&t);
                        xQueueSend(p_app_state->roommate_event_queue, &clock_update_event, portMAX_DELAY);

                        // Extract the events from the message
                        struct calendar_events_data msg_data = {
                            .num_events = rx_event.calendar_data.num_events
                        };
                        for (int i = 0; i < rx_event.calendar_data.num_events; i++) {
                            strptime(rx_event.calendar_data.events[i].start_time.bytes, "%Y-%m-%dT%H:%M:%SZ", &t);
                            msg_data.events[i].start = mktime(&t);

                            strptime(rx_event.calendar_data.events[i].end_time.bytes, "%Y-%m-%dT%H:%M:%SZ", &t);
                            msg_data.events[i].end = mktime(&t);
                        }
                        struct roommate_event calendar_event = {
                             .type = ROOMMATE_EVENT_UPDATE_CALENDAR_EVENTS,
                             .calendar_events_data = msg_data,
                        };
                        xQueueSend(p_app_state->roommate_event_queue, &calendar_event, portMAX_DELAY);

                    }
                    break;
                    case AWS_EVENT_REQUEST_CALENDAR_DATA:
                    break;
                    case AWS_EVENT_REQUEST_ROOM_HOLD:
                        {   
                            const char * topic = "reservation-request";
                            int msg_len = sprintf(outgoing_mqtt_msg_buffer, "{\"start\":%lu,\"finish\":%lu,\"boardId\":\"%s\"}", 
                                rx_event.room_hold_data.start,
                                rx_event.room_hold_data.finish,
                                clientId);

                            MQTTAgentPublishParams_t const publish_params = {
                                .pucTopic = (uint8_t *)topic, /**< The topic string on which the message should be published. */
                                .usTopicLength = strlen(topic),   /**< The length of the topic. */
                                .xQoS = eMQTTQoS1,           /**< Quality of Service (QoS). */
                                .pvData = outgoing_mqtt_msg_buffer,      /**< The data to publish. This data is copied into the MQTT buffers and therefore the user can free the buffer after the MQTT_AGENT_Publish call returns. */
                                .ulDataLength = msg_len,    /**< Length of the data. */
                            };
                            configPRINTF(("Attempting to publish MQTT msg to reserve room\r\n"));
                            MQTTAgentReturnCode_t publish_result = MQTT_AGENT_Publish(
                                p_app_state->mqtt_agent_handle,
                                &publish_params, 
                                pdMS_TO_TICKS(5000));
                        }
                    break;
                }
            }
            
            // configPRINTF(("Got either MQTT agent disconnected or an event came in for topic: %s\r\n", p_publish_parameters->pucTopic));


        }

    }
}

#define TOKEN_LENGTH(TOKEN) (TOKEN.end - TOKEN.start)
#define TOKEN_PTR_LENGTH(P_TOKEN) (P_TOKEN->end - P_TOKEN->start)

struct calendar_data extract_calendar_data(const char * data, size_t length) {
    struct calendar_data result;
    /*
    Example json data. Paste this into AWS console to test sending messages.
    {
        "time" : "2018-09-28T12:00:00Z",
        "events" : [
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:15:00Z", "e" : "2018-09-28T12:30:00Z" },
            { "s" : "2018-09-29T12:30:00Z", "e" : "2018-09-28T12:45:00Z" }
        ]
    }
    */

    // Initialize jasmine parse
    jsmn_parser parser;
    jsmn_init(&parser);
    const uint8_t num_tokens = 5 + (MAX_CALENDAR_EVENTS_PER_MESSAGE * 5); // 5 because (outer object + 4 strings)
    jsmntok_t tokens[num_tokens];

    // Execute the parse
    int parse_results = jsmn_parse(&parser, data, length, tokens, num_tokens);
    configPRINTF(("Jasmine found %d tokens in json blob\r\n", parse_results));

    if (parse_results > 0) {
        // Sanity check on the number of tokens received determine the number of events
        configASSERT((parse_results - 5) % 5 == 0);
        result.num_events = (parse_results - 5) / 5;

        // See the root object
        configASSERT(tokens[0].type == JSMN_OBJECT); 
        // See the timestamp key
        configASSERT(tokens[1].type == JSMN_STRING); 
        configASSERT(memcmp("time", &data[tokens[1].start], TOKEN_LENGTH(tokens[1])) == 0)
        // Extract the timestamp value
        configASSERT(tokens[2].type == JSMN_STRING); 
        configASSERT(TOKEN_LENGTH(tokens[2]) == sizeof(timestamp_t)); 
        memcpy(result.current_time.bytes, &data[tokens[2].start], TOKEN_LENGTH(tokens[2]));
        // See the events key
        configASSERT(tokens[3].type == JSMN_STRING); 
        configASSERT(memcmp("events", &data[tokens[3].start], TOKEN_LENGTH(tokens[3])) == 0)
        // See the events array
        configASSERT(tokens[4].type == JSMN_ARRAY);
        // The rest of the tokens should be events each event should be 5 tokens (outer object + 4 strings)

        jsmntok_t * token = &tokens[5];
        for (int i = 0; i < result.num_events; i++) {
            configPRINTF(("Unpacking event...\r\n", parse_results));
            // See the object that wraps the event
            configASSERT(token->type == JSMN_OBJECT); 
            token++;
            // See the start key
            configASSERT(token->type == JSMN_STRING); 
            configASSERT(memcmp("s", &data[token->start], TOKEN_PTR_LENGTH(token)) == 0)
            token++;
            // Extract the start timestamp
            configASSERT(token->type == JSMN_STRING); 
            configASSERT(TOKEN_PTR_LENGTH(token) == sizeof(timestamp_t)); 
            memcpy(result.events[i].start_time.bytes, &data[token->start], sizeof(timestamp_t));
            token++;
            // See the end key
            configASSERT(token->type == JSMN_STRING); 
            configASSERT(memcmp("e", &data[token->start], TOKEN_PTR_LENGTH(token)) == 0)
            token++;
            // Extract the end timestamp
            configASSERT(token->type == JSMN_STRING); 
            configASSERT(TOKEN_PTR_LENGTH(token) == sizeof(timestamp_t)); 
            memcpy(result.events[i].end_time.bytes, &data[token->start], sizeof(timestamp_t));
            token++;
        }
    } else {
        configPRINTF(("JASMINE JSON PARSE FAILED WITH ERROR: %d\r\n", result));
    }

    return result;
}

static MQTTBool_t handle_received_event_for_subscription( void * p_context,
                                   const MQTTPublishData_t * const p_publish_parameters ) {
    struct app_state * p_app_state = p_context;

    // configPRINTF(("Received event for topic: %s\r\n", p_publish_parameters->pucTopic));
    configPRINTF(("Data length is %d bytes\r\n", p_publish_parameters->ulDataLength));

    struct calendar_data extracted_calendar_data = extract_calendar_data(p_publish_parameters->pvData, p_publish_parameters->ulDataLength);
    configPRINTF(("Extracted %d calendar events!\r\n", extracted_calendar_data.num_events));

    struct aws_event event = {
        .type = AWS_EVENT_CALENDAR_DATA_RECEIVED,
        .calendar_data = extracted_calendar_data,
    };

    xQueueSend(p_app_state->aws_event_coordinator_queue, &event, portMAX_DELAY);
    xEventGroupSetBits(p_app_state->mqtt_agent_event_group, MQTT_EVENT_DATA_READY);

    /* The data was copied into the FreeRTOS message buffer, so the buffer
     * containing the data is no longer required.  Returning eMQTTFalse tells the
     * MQTT agent that the ownership of the buffer containing the message lies with
     * the agent and it is responsible for freeing the buffer. */
    return eMQTTFalse;
}