#include "writebuffer.h"

// returns last in queue and increments pointer
char* writebuffer_out(struct writebuffer* wb) {
    if (wb->in == wb->out) return NULL;
    wb->out++;
    if (wb->out == WRITE_BUFFER_SIZE) {
        wb->out = 0;
        return wb->data[WRITE_BUFFER_SIZE-1];
    }
    return wb->data[wb->out-1];
}

// returns address of first character in string for the next up buffer input
char* writebuffer_in(struct writebuffer* wb) {
    if (wb->in & WRITE_BUFFER_SIZE);

    // check if full
    // if full return null
    if (wb->in+1 == wb->out) return NULL;


    // if not full return current out and increment out
    wb->in++;
    if (wb->in == WRITE_BUFFER_SIZE) {
        wb->in = 0;
        return wb->data[WRITE_BUFFER_SIZE-1];
    }
    return wb->data[wb->in-1];
}