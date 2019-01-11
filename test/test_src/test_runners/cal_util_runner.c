#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(CalUtil)
{
  RUN_TEST_CASE(CalUtil, cal_util_get_led_statuses_indicates_current_meeting_for_first_led);
  RUN_TEST_CASE(CalUtil, cal_util_get_led_statuses_indicates_current_meeting_for_first_two_leds);
  RUN_TEST_CASE(CalUtil, cal_util_get_led_statuses_indicates_current_meeting_for_all_leds);
  RUN_TEST_CASE(CalUtil, cal_util_get_led_statuses_indicates_future_meeting_for_meetings_after_current_meeting);

}