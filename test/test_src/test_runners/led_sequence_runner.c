#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(LEDSequence)
{
  RUN_TEST_CASE(LEDSequence, newtons_cradle_does_not_exceed_max_frames_specified);
  RUN_TEST_CASE(LEDSequence, newtons_cradle_produces_frames_with_correct_led_pattern);
  
}