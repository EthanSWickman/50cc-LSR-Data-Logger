#ifndef PICOWBELL_SD_CARD_H
#define PICOWBELL_SD_CARD_H

/*
desc: initializes attached sd card for data transmission
parameters: N/A
output: N/A
*/
void picowbell_sd_card_init();

/*
desc: write line from input string buffer to the attached sd card
parameters: 
    buffer: string to write to output as line
output: writes text to SD card
*/
void picowbell_sd_card_write_line(char* buf);

#endif