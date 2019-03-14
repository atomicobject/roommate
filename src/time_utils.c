#include "time_utils.h"

void time_utils_set_current_time(time_t time) {
    time_t current_time = time_utils_get_current_time();
    bool time_is_not_set = current_time < 1552523417;
    bool new_time_is_earlier = time < current_time;
    if  (time_is_not_set || new_time_is_earlier) {
      struct timeval new_time = {
        .tv_sec = time,
        .tv_usec = 0,
      };
      settimeofday(&new_time, NULL); // NULL is for timezone

      configPRINTF( ( "New Time Set\r\n" ) );
    } else {
      configPRINTF( ( "Not setting time. Current time was better.\r\n" ) );
    }
}

time_t time_utils_get_current_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}
