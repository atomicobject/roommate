#include "unity.h"
#include "unity_fixture.h"
#include "led_sequence.h"
#include "led_utils.h"
#include "colors.h"

TEST_GROUP(LEDSequence);

TEST_SETUP(LEDSequence)
{
}

TEST_TEAR_DOWN(LEDSequence)
{
}

TEST(LEDSequence, newtons_cradle_does_not_exceed_max_frames_specified)
{
  struct led_sequence newtons_cradle = led_sequence_newtons_cradle_blue(NULL);

  uint32_t total_frames = 0;
  do {
    struct led_frame frame = newtons_cradle.executor(total_frames, total_frames * 2);
    if (frame.is_last_frame) {
      break;
    }
    total_frames++;
  } while (total_frames < 0xFFFFFFFF);

  TEST_ASSERT_LESS_THAN(newtons_cradle.max_frames, total_frames);
}

TEST(LEDSequence, newtons_cradle_produces_frames_with_correct_led_pattern)
{
  struct led_sequence newtons_cradle = led_sequence_newtons_cradle_blue(NULL);

  const uint32_t O = LED_OFF;
  const uint32_t I = BLUE;

  const uint32_t expected[15][8] = {
    { O, O, O, I, O, O, O, I },
    { O, O, O, I, O, O, I, O },
    { O, O, O, I, O, I, O, O },
    { O, O, O, I, I, O, O, O },
    { O, O, I, O, I, O, O, O },
    { O, I, O, O, I, O, O, O },
    { I, O, O, O, I, O, O, O },
    { I, O, O, O, I, O, O, O },
    { O, I, O, O, I, O, O, O },
    { O, O, I, O, I, O, O, O },
    { O, O, O, I, I, O, O, O },
    { O, O, O, I, O, I, O, O },
    { O, O, O, I, O, O, I, O },
    { O, O, O, I, O, O, O, I },
    { O, O, O, I, O, O, O, I },
  };

  uint32_t total_frames = 0;
  for (int i = 0; i < 15; i++) {
    struct led_frame frame = newtons_cradle.executor(total_frames, total_frames * 2);
    for (int led = 0; led < NUM_LEDS; led++) {
      TEST_ASSERT_EQUAL(expected[i][NUM_LEDS - led - 1], frame.new_state.leds[led]);
    }
    total_frames++;
  }
}
