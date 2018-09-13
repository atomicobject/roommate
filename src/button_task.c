
// GPIO usage based on the following example:
// https://github.com/espressif/esp-idf/blob/master/examples/peripherals/gpio/main/gpio_example_main.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "roommate_pinout.h"

static xQueueHandle gpio_evt_queue = NULL;

struct buttonEvent {
    uint32_t gpio_num;
    TickType_t tick_count;
};

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;

    /*
    I'd like to acquire the GPIO state (gpio_get_level()) here,
    but calling that from an ISR causes a crash :/

    The messages seem to be handled soon enough that the value hasn't changed.
    */

    struct buttonEvent event = {
        .gpio_num = gpio_num,
        .tick_count = xTaskGetTickCountFromISR()
    };

    xQueueSendFromISR(gpio_evt_queue, &event, NULL);
}

static void gpio_task_example(void* arg)
{
    struct buttonEvent buttonEvent;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &buttonEvent, portMAX_DELAY)) {
            int level = gpio_get_level(buttonEvent.gpio_num);

            printf("%d: GPIO[%d] intr, val: %d\n", buttonEvent.tick_count, buttonEvent.gpio_num, level );
        }
    }
}

#define GPIO_INPUT_BITMASK (1ULL<<BUTTON_INPUT_GPIO)
#define ESP_INTR_FLAG_DEFAULT 0

void beginHandlingButtonPresses(){
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_BITMASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.intr_type = GPIO_PIN_INTR_ANYEDGE;

    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(struct buttonEvent));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_INPUT_GPIO, gpio_isr_handler, (void*) BUTTON_INPUT_GPIO);
}