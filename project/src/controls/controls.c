#include "controls.h"
#include "pin_config.h"
#include "lcd.h"
#include "pico/rand.h"
#include "stdio.h"
#include "hardware/adc.h"

// control buttons use 100k, 47k, 22k, 10k ohm resistors in order from left button to right button

void main_menu();

void settings_menu() {
  lcd_write("<settings men  u! back togo back>");
  sleep_ms(5000);
  return;
}

void validation_menu() {
  lcd_write("<validation menu! back bac    k>");
  sleep_ms(5000);
  return;
}

void statistics_menu() {
  lcd_write("<statistics menu! back back    >");
  sleep_ms(5000);
  return;
}

void controls_init() {
  adc_init();
  adc_gpio_init(BUTTON_PIN);
  adc_select_input(1);
  gpio_pull_down(BUTTON_PIN);
}

void controls_startup() {
  lcd_write("   DataLogger          OS     ");
  sleep_ms(2000);
  lcd_write("-| DataLogger -||-     OS     |-");
  sleep_ms(300);
  lcd_write("|- DataLogger |--|     OS     -|");
  sleep_ms(300);
  lcd_write("-| DataLogger -||-     OS     |-");
  sleep_ms(300);
  lcd_write("|- DataLogger |--|     OS     -|");
  sleep_ms(300);
  lcd_write("-| DataLogger -||-     OS     |-");
  sleep_ms(300);
  lcd_write("-- DataLogger ----     OS     --");
  sleep_ms(2000);

  main_menu();
}

void main_menu() {
  char* menuOptions[] = {
    "<    Start     ><     Run      >",
    "<    Edit      ><    Config    >",
    "<    System    ><    Status    >",
    "<    Run       ><  Statistics  >",
  };
  void (*menuFunctions[4])(void) = {
    settings_menu,
    settings_menu,
    validation_menu,
    statistics_menu,
  };

  int selected = 0;
  lcd_write(menuOptions[selected]);

  while (true) {
    int read = adc_read();
    switch(adc_read()) {
      case 400 ... 999:
        lcd_write(menuOptions[--selected % 4]);
        while (adc_read() > 400) continue;
        break;
      case 1000 ... 1999:
        break;
      case 2000 ... 2999:
        (*menuFunctions[selected]);
        lcd_write(menuOptions[selected]);
        while (adc_read() > 400) continue;
        break;
      case 3000 ... 3999:
        lcd_write(menuOptions[++selected % 4]);
        while (adc_read() > 400) continue;
        break;
      default: 
        continue;
    }
  }
}


