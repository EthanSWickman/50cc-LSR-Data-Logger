#include <stdio.h>

#include "pico/stdlib.h"
#include "hw_config.h"

#include "picowbell_sd_card.h"

void picowbell_sd_card_init(FATFS* fs) {
    sd_init_driver();
    f_mount(fs, "0:", 1);
}

void picowbell_sd_card_new_log(FIL* f) {
    // close old file
    f_close(f);
    
    // find new file number
    uint i = 0;
    char testFile[11];
    while (1) {
        sprintf(testFile, "log%d.csv", i);
        if (f_stat(testFile, NULL) == FR_NO_FILE) {
            break;
        }
        i++;
    }

    // open new file
    f_open(f, testFile, FA_WRITE | FA_OPEN_APPEND);
}