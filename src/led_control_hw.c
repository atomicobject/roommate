#include "led_control_hw.h"
#include "driver/rmt.h"
#include "esp_log.h"
#include "FreeRTOS.h"

#define LED_RMT_TX_CHANNEL RMT_CHANNEL_0
#define LED_RMT_TX_GPIO 18
#define SAMPLE_CNT  (10)

#define NUM_LEDS 8
#define BITS_PER_LED_CMD 24 
#define LED_BUFFER_ITEMS ((NUM_LEDS * BITS_PER_LED_CMD)) // One extra for low period at  end of sequence


// typedef struct {
//     union {
//         struct {
//             uint32_t duration0 :15;
//             uint32_t level0 :1;
//             uint32_t duration1 :15;
//             uint32_t level1 :1;
//         };
//         uint32_t val;
//     };
// } rmt_item32_t;


#define RGB(R, G, B)  ((((uint32_t)R) << 16) | (((uint32_t)G) << 8) | B)

#define T0H 14 // 18 works
#define T1H 52 // 40 works
#define TL  52 // 24 works

rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS];

void init_led_data_buffer(int a);
void rmt_send_finished_isr(void *p_data);

void led_control_hw_init()
{
  // led_data_buffer = (rmt_item32_t*)malloc(LED_BUFFER_ITEMS * sizeof(rmt_item32_t));
  rmt_config_t config;
  config.rmt_mode = RMT_MODE_TX;
  config.channel = LED_RMT_TX_CHANNEL;
  config.gpio_num = LED_RMT_TX_GPIO;
  config.mem_block_num = 3;
  config.tx_config.loop_en = false;
  // enable the carrier to be able to hear the Morse sound
  // if the RMT_TX_GPIO is connected to a speaker
  config.tx_config.carrier_en = false;
  config.tx_config.idle_output_en = true;
  config.tx_config.idle_level = 0;

  // config.tx_config.carrier_duty_percent = 50;
  // set audible career frequency of 611 Hz
  // actually 611 Hz is the minimum, that can be set
  // with current implementation of the RMT API
  // config.tx_config.carrier_freq_hz = 611;
  // config.tx_config.carrier_level = 1;
  // set the maximum clock divider to be able to output
  // RMT pulses in range of about one hundred milliseconds
  config.clk_div = 2; // Can be 1-255. The RMT source clock is typically APB CLK, 80Mhz by default.

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
  // ESP_ERROR_CHECK(rmt_translator_init(config.channel, u8_to_rmt));
  init_led_data_buffer(0);

  // ESP_ERROR_CHECK(rmt_isr_register(rmt_send_finished_isr, NULL, 0, NULL));
}

// void rmt_send_finished_isr(void *p_data) {
//   configPRINTF(("LED Control rmt_write_items FINISHED!\r\n") );
// }

 // AO_GREEN: 0x2ACAC1
void init_led_data_buffer(int a) 
{

  uint32_t colors[8] = {
    RGB(0x77, 0x11, 0x11),
    RGB(0x11, 0x77, 0x11),
    RGB(0x11, 0x11, 0x77),
    RGB(0x77, 0x77, 0x11),
    RGB(0x77, 0x11, 0x11),
    RGB(0x11, 0x77, 0x11),
    RGB(0x11, 0x11, 0x77),
    RGB(0x77, 0x77, 0x11),
  };


  for (uint32_t led = 0; led < NUM_LEDS; led++) {
    // uint32_t bits_to_send = RGB(0x2A, 0xCA, 0xC1);
    uint32_t bits_to_send = colors[(a + led) % 8];
    // uint32_t bits_to_send = 0x000001;
    // if (a % 2 == 0) {
    //   bits_to_send = RGB(0x00, 0xFF, 0x00);
    // }
    // if (a % 3 == 0) {
    //   bits_to_send = RGB(0xFF, 0x00, 0x00);
    // }

    // configPRINTF(("Sending %x \n", bits_to_send));
    uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);
    for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++) {
      uint32_t bit_is_set = bits_to_send & mask;
      led_data_buffer[led * BITS_PER_LED_CMD + bit] = bit_is_set ?  // + 1 to skip the first one for the reset
                                                      (rmt_item32_t){{{T1H, 1, TL, 0}}} : 
                                                      (rmt_item32_t){{{T0H, 1, TL, 0}}};
      mask >>= 1;
    }
  }

  // Add a LOW period at the start and end of the transmission
  // led_data_buffer[LED_BUFFER_ITEMS - 1] = (rmt_item32_t){{{1, 1, 45, 0}}};
}

int led_cycle = 0;


void led_control_hw_test() {
  configPRINTF(("LED Control starting rmt_write_items...\r\n") );
  ESP_ERROR_CHECK(rmt_write_items(LED_RMT_TX_CHANNEL, led_data_buffer, LED_BUFFER_ITEMS, false));
  ESP_ERROR_CHECK(rmt_wait_tx_done(LED_RMT_TX_CHANNEL, portMAX_DELAY));
  init_led_data_buffer(led_cycle);
  led_cycle++;
  configPRINTF(("Transmission complete...\r\n") );
}


