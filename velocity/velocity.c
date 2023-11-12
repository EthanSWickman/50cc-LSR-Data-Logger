#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "ll.h"

#define HES_PIN 22 //on the upper right on the pinout
#define ALARM0_INTERUPT 0 //the interrupt number for alarm 0
#define PROCCESOR1_INTERRUPT 16 
#define TIME_INTERVAL 1000000 //in us
#define MAX_ROTATION_QUEUE_ENTRIES 100000

volatile uint num_timer_interrupts = 1;
volatile uint LED_PIN = PICO_DEFAULT_LED_PIN;
volatile uint num_rotations = 0;
volatile bool rot_incremented_last_iter = false;
ll* rotations_queue;

void sd_card_write(uint front) {
    //write the rotations/time to the sd card
    char sd_card_string[11];
    sprintf(sd_card_string, "%i", front);
    printf("writing (%i) to sd card...\n", front);
    printf("put sd card write code here (%i)\n", front);
}

void core1_entry() {
    printf("in other core... \n");
    multicore_fifo_clear_irq();
    rotations_queue = buildll();

    while(true) { //change back to while true for production, multicore_fifo_rvalid() only for debugging
        if (multicore_fifo_rvalid()) {
            printf("core 1 recieved rotations from core 0, writing to rotations queue... \n");
            uint curr_rotations = multicore_fifo_pop_blocking();
            //put the data we recieve from other core into a queue (end of ll)
            if (rotations_queue->len >= MAX_ROTATION_QUEUE_ENTRIES) {
                printf("rotation data queue has too many (%i) entires, write more data to sd card to make room for more data\n", rotations_queue->len);
            }
            else {
                appendNode(rotations_queue, curr_rotations);
                printll(rotations_queue);
                printf("wrote rotations (%i) to queue!\n", curr_rotations);
            }
        }
        
        if (rotations_queue->len > 0) {
            //dequeue the first rotation value (use linked list)
            uint front = rotations_queue->head->val;
            removeNode(rotations_queue, 0);
            printll(rotations_queue);
            sd_card_write(front);
        }
    }
}

/*
//write a function to shutdown whole system and write reamining data to an sd card
//call when button is pressed to shut the system down
void core1_cleanup() {

}
*/
void alarm0_irq() {
    num_timer_interrupts++; //make sure the next interrupt is the same amount of time from the last
    hardware_alarm_set_target(0, TIME_INTERVAL * num_timer_interrupts); 
    printf("writing rotations to other core... \n");
    //give the other core the number of rotations
    multicore_fifo_push_blocking(num_rotations);
    num_rotations = 0; //reset number of rotations for next cycle
    return;
}

int main(void) {
    printf("running startup routine... \n");
    stdio_init_all();
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(HES_PIN, GPIO_IN);
    //if no external resistor pull HES up/down in software to make sure
    //there are no floating values

    hardware_alarm_claim(ALARM0_INTERUPT);
    hardware_alarm_set_target(ALARM0_INTERUPT, TIME_INTERVAL); //the timer is incremented in microseconds, so 1000000 ticks is one second
    hardware_alarm_set_callback(ALARM0_INTERUPT, alarm0_irq); //set our irq for the alarm

    // Initialize SD card
    
    multicore_launch_core1(core1_entry);
    //HES shouldn't bounce, if it does try doing external debouncing to have minimal
    //effect on detectable rpms
    while(true) {
        //only increment counter if the reading was low the previous iteration
        //this stops repeated readings 
        if (gpio_get(HES_PIN) == 1 && !rot_incremented_last_iter) { 
            puts("incrementing number of rotations");
            num_rotations++;
            rot_incremented_last_iter = true;
        }
        else if (gpio_get(HES_PIN) == 0) {
            rot_incremented_last_iter = false;
        }
    }
}