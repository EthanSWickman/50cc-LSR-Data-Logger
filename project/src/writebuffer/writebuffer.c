#include "writebuffer.h"
#include <stdio.h>
#include "pico/multicore.h"


char* writebuffer_out(struct writebuffer* wb) {
    // return null on empty writebuffer
    if (wb->in == wb->out) {
        return NULL;
    } 

    // return pointer to start of stored string
    char* returnString = wb->data[wb->out++ & (WRITE_BUFFER_SIZE - 1)];
    return returnString;
}

char* writebuffer_in(struct writebuffer* wb) {
    // return null on full writebuffer
    if (wb->in - wb->out == WRITE_BUFFER_SIZE) {
        return NULL;
    }

    // return pointer to start of space for new string to be written
    char* returnString = wb->data[wb->in++ & (WRITE_BUFFER_SIZE - 1)];
    return returnString;
}