#include "hes.h"

uint pwm_counter_setup(uint gpio, funcPtr wrapFunc) {
    //DO NOT try to use an even GPIO (gpio 0, 2, 4 etc) pin, will not work
    //printf("pwm is on channel b: %b\n", pwm_gpio_to_channel(gpio) == PWM_CHAN_B);
    //convert the gpio number into a PWM slice
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_config cfg = pwm_get_default_config();
    //set the pwm to detect rising edge 
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_config_set_clkdiv(&cfg, 1.f);
    pwm_init(slice_num, &cfg, false);
    //tell gpio to be assigned to a PWM slice 
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    //setup nterrupt handler to deal with overflows
    pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, wrapFunc);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_set_enabled(slice_num, true);
    return slice_num; 
}

uint calc_rotations(volatile uint pwm_slice, volatile uint* wrap_count_ptr) {
    //calculate current rotations to add to the write buffer
    uint counter = pwm_get_counter(pwm_slice);
    //printf("current pwm counter value %i for slice number %i\n", counter, pwm_slice);
    uint rotations = (*wrap_count_ptr * PWM_COUNTER_MAX) + counter;
    //printf("total rotations: %i\n", rotations);
    *wrap_count_ptr = 0; //reset count for next time interval
    pwm_set_counter(pwm_slice, 0); 
    return rotations;
}
