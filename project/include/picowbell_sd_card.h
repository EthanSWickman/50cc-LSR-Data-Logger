#ifndef PICOWBELL_SD_CARD_H
#define PICOWBELL_SD_CARD_H

#include "ff.h"

/*
desc: initializes attached sd card for data transmission and mounts filesystem
parameters: 
    fs: FatFS filesystem object pointer
output: sets fs to the mounted SD card's filesystem
*/
void picowbell_sd_card_init(FATFS* fs);

/*
desc: saves old log file and returns a new log file to write to 
parameters: 
    f: pointer to file object of old log file
    close_old: true/false, set to true to close a file already existing on f
output: sets f to newly opened log file
*/
void picowbell_sd_card_new_log(FIL* f, bool close_old);

#endif