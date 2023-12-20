#ifndef HES_H
#define HES_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>

#define PWM_COUNTER_MAX 65535 // value pwm counter overflows at

//use to set up hardware wrapping fucntion
typedef void (*funcPtr);

//setup PWM counter
/*
the basic idea of the counter is this: count the number of edges detected from 
the HES using a PWM slice. since the counter is only 16 bits, it will 
overflow when its value is 2^16 - 1 and you try to increment it.
this is why we keep track of the number of wraps using an interrupt handler.
code is heavily inspired by this github repository reccomended by ethan: 
https://github.com/richardjkendall/rp2040-freq-counter/blob/main/pwm/main.c
since we already have a timing system in place implenting a pps (pulse per second)
using a gpio was not needed.
*/ 
uint pwm_counter_setup(uint gpio, funcPtr wrapFunc);

/*
actual velocity calculation is done in other core so we dont affect the 
max detectable rpms. it would have to get to above 65k rpms to matter,
so we are probablly fine with or without the wrap checking. including just 
to be safe, will remove if multiply is negatively impacting performance.
*/
uint calc_rotations(volatile uint pwm_slice, volatile uint* wrap_count_ptr);
#endif