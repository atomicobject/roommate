#include "time_utils.h"

void time_utils_set_current_time(time_t time) {
    configPRINTF(("Roommate task setting current time!\r\n"));
    struct timeval new_time = {
      .tv_sec = time,
      .tv_usec = 0,
    };
    settimeofday(&new_time, NULL); // NULL is for timezone
}

time_t time_utils_get_current_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}
