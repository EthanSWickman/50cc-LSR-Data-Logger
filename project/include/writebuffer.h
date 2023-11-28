#ifndef WRITEBUFFER_H 
#define WRITEBUFFER_H

#define LOG_SIZE 20
#define WRITE_BUFFER_SIZE 32 

#include <stdlib.h>

#include "pico/stdlib.h"

struct writebuffer {
    uint in;
    uint out;
    char data[WRITE_BUFFER_SIZE][LOG_SIZE];
};

char* writebuffer_out(struct writebuffer* wb);

char* writebuffer_in(struct writebuffer* wb);

#endif