#include "limit_switch.h"

/*
char ls_right_state = HIGH;
char ls_left_state = HIGH;
char last_ls_right_state = HIGH;
char last_ls_left_state = HIGH;
*/

void limit_switch_setup(){
    gpio_init(LSPinRIGHT);
    gpio_pull_up(LSPinRIGHT);
    
    gpio_init(LSPinLEFT);
    gpio_pull_up(LSPinLEFT);
}