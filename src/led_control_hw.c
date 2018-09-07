#include "led_control_hw.h"
#include "driver/rmt.h"
#include "esp_log.h"
#include "FreeRTOS.h"
#include "roommate_pinout.h"

#define LED_RMT_TX_CHANNEL RMT_CHANNEL_0

#define NUM_LEDS 8
#define BITS_PER_LED_CMD 24 
#define LED_BUFFER_ITEMS ((NUM_LEDS * BITS_PER_LED_CMD)) // One extra for low period at  end of sequence

// Just leaving this here for reference. It's defined somewhere in the rmt driver.
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



// These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
#define T0H 14  // 0 bit high time
#define T1H 52  // 1 bit high time
#define TL  52  // low time for either bit

rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS];

void rmt_send_finished_isr(void *p_data);

void setBits(uint32_t bits_to_send, uint32_t led){
  uint32_t mask = 1 << (BITS_PER_LED_CMD - 1);
  for (uint32_t bit = 0; bit < BITS_PER_LED_CMD; bit++) {
    uint32_t bit_is_set = bits_to_send & mask;
    led_data_buffer[led * BITS_PER_LED_CMD + bit] = bit_is_set ?  // + 1 to skip the first one for the reset
                                                    (rmt_item32_t){{{T1H, 1, TL, 0}}} : 
                                                    (rmt_item32_t){{{T0H, 1, TL, 0}}};

    mask >>= 1;
  }

  // We could use something like this to add a LOW period at the start and end of the transmission rather than a task delay
  // led_data_buffer[LED_BUFFER_ITEMS - 1] = (rmt_item32_t){{{1, 1, 45, 0}}};
}

void rotate_colors(int frame) 
{
  uint32_t colors[8] = {
    0x771111,
    0x117711,
    0x111177,
    0x777711,
    0x771111,
    0x117711,
    0x111177,
    0x777711,
  };

  for (uint32_t led = 0; led < NUM_LEDS; led++) {
    uint32_t bits_to_send = colors[(frame + led) % 8];

    setBits(bits_to_send, led);
  }
}

void zipzap(int frame) 
{
  int num_frames = (NUM_LEDS*2 - 2);
  int animation_frame = frame % num_frames;
  int pos = animation_frame < NUM_LEDS ? animation_frame : num_frames - animation_frame;

  // configPRINTF(("frame %d pos %d\n",animation_frame, pos) );
  for (uint32_t led = 0; led < NUM_LEDS; led++) {
    uint32_t bits_to_send = led == pos ? 0xFFFFFF : 0;

    setBits(bits_to_send, led);
  }
}

void (*init_led_data_buffer)(int frame) = zipzap;

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
  init_led_data_buffer(0);
}

int led_cycle = 0;

void led_control_hw_test() {
  configPRINTF(("LED Control starting rmt_write_items...\r\n") );
  ESP_ERROR_CHECK(rmt_write_items(LED_RMT_TX_CHANNEL, led_data_buffer, LED_BUFFER_ITEMS, false));
  ESP_ERROR_CHECK(rmt_wait_tx_done(LED_RMT_TX_CHANNEL, portMAX_DELAY));
  led_cycle++;
  init_led_data_buffer(led_cycle);
  configPRINTF(("Transmission complete...\r\n") );
}


