#ifndef PICOWBELL_PCF8520_H
#define PICOWBELL_PCF8520_H

/*
functions for interacting with pcf8520 clock 
*/

#include "pico/stdlib.h"

/*
desc: initializes pcf8520 clock pins, must be called before anything else
parameters: N/A
output: N/A
*/
void picowbell_pcf8520_init();

/*
desc: writes current pcf8520 clock time to input buffer
parameters: 
    buffer: string to fill with current time (18 bytes long) 
output: updates buffer with current time formatted as "HH:MM:SS MM/DD/YY"
*/
void picowbell_pcf8520_get_time_string(char* buffer);

/*
desc: returns time struct with current time
parameters: N/A
output: returns time struct with values for current pcf8520 time
*/
datetime_t picowbell_pcf8520_get_time();


/*
desc: sets the clock
parameters: 
    time: time struct with members representing time to set
*/
void picowbell_pcf8520_set_time(datetime_t time); 

void picowbell_pcf8520_wait_next_second(datetime_t* latestTime);

#endif