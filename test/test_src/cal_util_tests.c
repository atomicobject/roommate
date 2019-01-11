#include "unity.h"
#include "unity_fixture.h"
#include "cal_util.h"

void setup_led_boundaries(struct led_boundaries * p_boundaries, time_t start, time_t period);

TEST_GROUP(CalUtil);

TEST_SETUP(CalUtil) { }

TEST_TEAR_DOWN(CalUtil) { }

TEST(CalUtil, cal_util_get_led_statuses_indicates_current_meeting_for_first_led)
{
  struct led_boundaries boundaries = { };
  setup_led_boundaries(&boundaries, 100, 100);
  struct calendar_events_data events = { .num_events = 1 };
  events.events[0].time_range = (struct time_range){
    .start = 100,
    .end = 200,
  };

  struct led_statuses result = cal_util_get_led_statuses(&boundaries, &events);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[0]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[1]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[2]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[3]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[4]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[5]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[6]);
}

TEST(CalUtil, cal_util_get_led_statuses_indicates_current_meeting_for_first_two_leds)
{
  struct led_boundaries boundaries = { };
  setup_led_boundaries(&boundaries, 100, 100);
  struct calendar_events_data events = { .num_events = 1 };
  events.events[0].time_range = (struct time_range){
    .start = 100,
    .end = 300,
  };

  struct led_statuses result = cal_util_get_led_statuses(&boundaries, &events);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[0]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[1]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[2]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[3]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[4]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[5]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[6]);
}

TEST(CalUtil, cal_util_get_led_statuses_indicates_current_meeting_for_all_leds)
{
  struct led_boundaries boundaries = { };
  setup_led_boundaries(&boundaries, 100, 100);
  struct calendar_events_data events = { .num_events = 1 };
  events.events[0].time_range = (struct time_range){
    .start = 100,
    .end = 800,
  };

  struct led_statuses result = cal_util_get_led_statuses(&boundaries, &events);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[0]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[1]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[2]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[3]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[4]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[5]);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[6]);
}

TEST(CalUtil, cal_util_get_led_statuses_indicates_future_meeting_for_meetings_after_current_meeting)
{
  struct led_boundaries boundaries = { };
  setup_led_boundaries(&boundaries, 100, 100);
  struct calendar_events_data events = { .num_events = 3 };
  events.events[0].time_range = (struct time_range){
    .start = 100,
    .end = 200,
  };

  events.events[1].time_range = (struct time_range){
    .start = 300,
    .end = 400,
  };

  events.events[2].time_range = (struct time_range){
    .start = 500,
    .end = 600,
  };

  struct led_statuses result = cal_util_get_led_statuses(&boundaries, &events);
  TEST_ASSERT_EQUAL(LED_STATUS_CURRENT_MEETING, result.leds[0]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[1]);
  TEST_ASSERT_EQUAL(LED_STATUS_FUTURE_MEETING, result.leds[2]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[3]);
  TEST_ASSERT_EQUAL(LED_STATUS_FUTURE_MEETING, result.leds[4]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[5]);
  TEST_ASSERT_EQUAL(LED_STATUS_ROOM_AVAILABLE, result.leds[6]);
}

void setup_led_boundaries(struct led_boundaries * p_boundaries, time_t start, time_t period) {
  for (int i = 0; i < NUM_LEDS; i++) {
    p_boundaries->leds[i] = (struct time_range){
      .start = start + (period * i),
      .end = start + (period * i) + period - 1
    };
  }
}
