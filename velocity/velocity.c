#include <stdio.h>
#include "pico/stdlib.h"

#define HES_PIN 28 //look at pinout
#define ALARM0_INTERUPT 0 //the interrupt number for alarm 0

volatile uint num_timer_interrupts = 1;
volatile uint LED_PIN = PICO_DEFAULT_LED_PIN;
volatile uint num_rotations = 0;
volatile bool rot_incremented_last_iter = false;

void second_core_routine() {
    //put the data we recieve into a queue
    //dequeue the first rotation value (use linked list)
    //(optional, can be done after) divide the current rotation value by the 
    //time in between timer interrupts to get the amount of 
    //rotations per time interval
    //write the rotations/time unit to the sd card
}

void second_core_shutdown_routine() {
    //do same thing as normal routine except: 
    //do not take in any more data from the first core
    //write everything left in queue to sd card
    //send signal to turn off the pico
}

void alarm0_irq() {
    num_timer_interrupts++; //make sure the next interrupt is the same amount of time from the last
    hardware_alarm_set_target(0, 1000000 * num_timer_interrupts); 
    //give the other core the number of rotations
    multicore_fifo_push_blocking(num_rotations);
    num_rotations = 0; //reset number of rotations for next cycle
    return;
}

int main(void) {
    stdio_init_all();
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(HES_PIN, GPIO_IN);
    //if no external resistor pull HES up/down in software to make sure
    //there are no floating values

    hardware_alarm_claim(ALARM0_INTERUPT);
    hardware_alarm_set_target(ALARM0_INTERUPT, 1000000); //the timer is incremented in microseconds, so 1000000 ticks is one second
    hardware_alarm_set_callback(ALARM0_INTERUPT, alarm0_irq); //set our irq for the alarm
    
    //HES shouldn't bounce, if it does try doing external debouncing to have minimal
    //effect on detectable rpms
    while(true) {
        //only increment counter if the reading was low the previous iteration
        if (gpio_get(HES_PIN) == 1 && !rot_incremented_last_iter) { 
            num_rotations++;
            rot_incremented_last_iter = true;
        }
        else if (gpio_get(HES_PIN) == 0) {
            rot_incremented_last_iter = false;
        }
    }
}