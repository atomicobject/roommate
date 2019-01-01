#include "cal_util.h"
#include "led_utils.h"
#include "colors.h"

void cal_util_get_led_boundaries(time_t const current_timestamp, struct led_boundaries * const p_boundaries) {
    const int seconds_per_chunk = 60 * 15;
    time_t seconds_remaining_in_chunk = current_timestamp % seconds_per_chunk;
    time_t chunk_start = current_timestamp - seconds_remaining_in_chunk;

    for(int i = 0; i < NUM_LEDS; i++) {
        p_boundaries->leds[i] = (struct time_range){
            .start = chunk_start,
            .end = chunk_start + seconds_per_chunk - 1
        };
        // configPRINTF(("LED chunk start: %d, end: %d!\r\n", led_time_chunks[i].start, led_time_chunks[i].end));
        chunk_start += seconds_per_chunk;
    }
}

struct led_statuses cal_util_get_led_statuses(struct led_boundaries const * const p_boundaries,
                                              struct calendar_events_data * const p_events) {

    bool first_event_is_ongoing = false;
    struct led_statuses result;
    for (int led = 0; led < NUM_LEDS; led++) {
        struct time_range this_led = p_boundaries->leds[led];
        result.leds[led] = LED_STATUS_ROOM_AVAILABLE;
        // configPRINTF(("Checking LED chunk %d to %d...\r\n", p_this_led->start, p_this_led->end ));

        for (uint32_t event_index = 0; event_index < p_events->num_events; event_index++) {
            struct time_range this_event = p_events->events[event_index];
            // configPRINTF(("  Checking event %d to %d...\r\n", this_event.start, this_event.end ));
            if (this_event.start > this_led.end) {
                // configPRINTF(("    Event start (%d) is greater than LEDs end (%d)!\r\n", this_event.start, p_this_led->end ));
                // This event is too far in the future. No event coincides with this LED.
                break;
            }
            if (this_event.end - 1 < this_led.start) {  // -1 from event end because google calendar events overlap
                // This event happened before this LEDs time window. Check the next event.
                // configPRINTF(("    Event end (%d) is before LEDs start (%d)!\r\n", this_event.end, p_this_led->start ));
                continue; 
            }
            // The event must fall within this LEDs time window
            if (led == 0) {
                first_event_is_ongoing = true;
                result.leds[led] = LED_STATUS_CURRENT_MEETING;
            } else if (first_event_is_ongoing && event_index == 0) {
                result.leds[led] = LED_STATUS_CURRENT_MEETING;
            } else {
                result.leds[led] = LED_STATUS_FUTURE_MEETING;
            }
            break;
        }

        // configPRINTF(("LED[%d] brightness: %d!\r\n", led, p_this_led->color));
    }
    return result;
}
 
int cal_util_get_color_for_status(enum led_status status) {
    switch (status) {
        case LED_STATUS_CURRENT_MEETING:
            return AO_YELLOW;
        case LED_STATUS_ROOM_AVAILABLE:
            return AO_GREEN;
        case LED_STATUS_FUTURE_MEETING:
            return AO_RED;
    }
    return LED_OFF;
}