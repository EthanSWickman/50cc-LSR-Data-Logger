#ifndef WRITEBUFFER_H 
#define WRITEBUFFER_H

#define LOG_SIZE 35 // maximum string size for storing a single log
#define WRITE_BUFFER_SIZE 32 // must be 2^n for some integer n, represents maximum number of strings in the writebuffer

#include <stdlib.h>

#include "pico/stdlib.h"

/*
desc: circular buffer to store logs in between cores 0 and 1
members: 
    in: index of next string to be written to in buffer
    out: index of next string to be read from queue 
    data: array to store strings
*/
struct writebuffer {
    uint in;
    uint out;
    char data[WRITE_BUFFER_SIZE][LOG_SIZE];
};

/*
desc: retrieve the next string to be output by the writebuffer
parameters: 
    wb: writebuffer object
output: returns a pointer to the first character in the oldest string stored in the writebuffer
*/
char* writebuffer_out(struct writebuffer* wb);

/*
desc: retrieve the address of the next available space for a string to be written to the writebuffer
parameters: 
    wb: writebuffer object
output: returns a pointer to the next available space for a string in the writebuffer
*/
char* writebuffer_in(struct writebuffer* wb);

#endif