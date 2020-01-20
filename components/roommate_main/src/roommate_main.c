#include "roommate_main.h"
#include "led_control.h"
#include "app_state.h"
#include "experiments_task.h"
#include "button_task.h"
#include "mqtt_agent_manager.h"
#include "aws_event_coordinator.h"
#include "roommate.h"
#include "led_sequence.h"

#include "queue.h"
#include "iot_wifi.h"
#include "esp_wifi.h" // Needed for definition of ESP_IF_WIFI_STA 
#include "esp_interface.h" 

#include <stdbool.h>
void start_connecting_to_wifi_animation(struct app_state const * const p_app_state);

struct app_state app_state;

void roommate_app_begin(void_func connect_wifi) {
    app_state.led_control_queue = led_control_start_controlling_leds(&app_state);
    start_connecting_to_wifi_animation(&app_state);

    app_state.roommate_event_queue = roommate_begin(&app_state);

    /* Connect to the wifi before running the demos */
    connect_wifi();

    esp_wifi_get_mac(ESP_IF_WIFI_STA, app_state.mac_address.mac);

    button_task_begin_handling_presses(&app_state);

    #if 0
    mqtt_agent_manager_begin(&app_state);
    app_state.aws_event_coordinator_queue = aws_event_coordinator_start_coordinating(&app_state);
    #endif

    vTaskDelay(pdMS_TO_TICKS(1000));
    experiments_task_start(&app_state);
}

bool should_continue_wifi_animation(struct app_state const * const p_app_state) {
    return WIFI_IsConnected() == pdFALSE;
}

void start_connecting_to_wifi_animation(struct app_state const * const p_app_state) {
    struct led_control_request msg = {
        .type = LED_CONTROL_SEQUENCE_REQUEST,
        .sequence_request_data = led_sequence_newtons_cradle_blue(should_continue_wifi_animation),
    };
    xQueueSend(app_state.led_control_queue, &msg, portMAX_DELAY);
}