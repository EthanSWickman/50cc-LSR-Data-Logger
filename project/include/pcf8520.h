#ifndef PCF8520_H
#define PCF8520_H

/*
functions for interacting with pcf8520 clock 
*/

#include "pico/stdlib.h"

// representation of time, down to the second
struct time {
    uint8_t years;
    uint8_t months;
    uint8_t days;
    uint8_t weekDays;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

/*
desc: initializes pcf8520 clock pins, must be called before anything else
parameters: N/A
output: N/A
*/
void pcf8520_init();

/*
desc: writes current pcf8520 clock time to input buffer
parameters: char buffer (18 bytes long)
output: updates buffer with current time formatted as "HH:MM:SS MM/DD/YY"
*/
void pcf8520_get_time_string(char* buffer);

/*
desc: returns time struct with current time
parameters: N/A
output: returns time struct with values for current pcf8520 time
*/
struct time pcf8520_get_time();

#endif