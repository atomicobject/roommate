#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(LEDLogic);

TEST_SETUP(LEDLogic)
{
}

TEST_TEAR_DOWN(LEDLogic)
{
}

TEST(LEDLogic, all_leds_should_be_green_when_no_meetings_scheduled)
{
  TEST_ASSERT_EQUAL(0, 0);
}
