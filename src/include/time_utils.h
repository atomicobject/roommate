#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include "FreeRTOS.h"
#include <sys/time.h>

void time_utils_set_current_time(time_t time);
time_t time_utils_get_current_time(void);

#endif