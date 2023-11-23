#include "writebuffer.h"
#include <stdio.h>
#include "pico/multicore.h"


// may need to add mutex on in and out indices
// returns last in queue and increments out marker 
char* writebuffer_out(struct writebuffer* wb) {
    if (wb->in == wb->out) {
        return NULL;
    } 
    printf("attempting to take from buffer, in: %d, out: %d, addr: %x\n", wb->in, wb->out, wb->data[wb->out & (WRITE_BUFFER_SIZE - 1)]);
    char* returnString = wb->data[wb->out++ & (WRITE_BUFFER_SIZE - 1)];
    return returnString;
}

// may need to add mutex on in and out indices
// returns address of first character in string for the next up buffer input
char* writebuffer_in(struct writebuffer* wb) {
    printf("attempting to insert to buffer, in: %d, out: %d, addr: %x\n", wb->in, wb->out, wb->data[wb->in & (WRITE_BUFFER_SIZE - 1)]);
    if (wb->in - wb->out == WRITE_BUFFER_SIZE) {
        return NULL;
    }
    char* returnString = wb->data[wb->in++ & (WRITE_BUFFER_SIZE - 1)];
    return returnString;
}