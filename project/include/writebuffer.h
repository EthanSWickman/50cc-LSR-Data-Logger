#ifndef WRITEBUFFER_H 
#define WRITEBUFFER_H

#define LOG_LINE_SIZE 100
#define WRITE_BUFFER_SIZE 128 // must be power of 2 for bit shifting mask to work!

#include <stdlib.h>

#include "pico/stdlib.h"

struct writebuffer {
    uint in;
    uint out;
    char data[WRITE_BUFFER_SIZE][LOG_LINE_SIZE];
};

char* writebuffer_out(struct writebuffer* wb);

char* writebuffer_in(struct writebuffer* wb);

#endif