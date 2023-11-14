#include <stdio.h>

#include "pico/stdlib.h"
#include "hw_config.h"
#include "ff.h"

#include "picowbell_sd_card.h"

const int MISO_PIN = 16;
const int CS_PIN = 17;
const int SCK_PIN = 18;
const int MOSI_PIN = 19;

void picowbell_sd_card_init() {
}

void picowbell_sd_card_write_line(char* buf) {
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;

    fr = f_mount(&fs, "0:", 1);

    fr = f_open(&fil, "testfile", FA_WRITE || FA_CREATE_ALWAYS);

    ret = f_printf(&fil, "This is a test\n");
    ret = f_printf(&fil, "of writing to an SD card.\n");

    fr = f_close(&fil);

    f_unmount("0:");

    //printf("writing %s to sd card...\n", buf);
}