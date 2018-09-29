#ifndef CALENDAR_EVENT_H
#define CALENDAR_EVENT_H
#include <stdint.h>

typedef struct timestamp { 
    uint8_t bytes[20]; 
} timestamp_t; // 2018-01-01T12:00:00Z

struct calendar_event {
    timestamp_t start_time;
    timestamp_t end_time;
};

#endif