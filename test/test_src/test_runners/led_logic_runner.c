#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(LEDLogic)
{
  RUN_TEST_CASE(LEDLogic, all_leds_should_be_green_when_no_meetings_scheduled);
}