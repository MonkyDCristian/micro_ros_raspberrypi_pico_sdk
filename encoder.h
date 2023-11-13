#include "pico/stdlib.h"
#include "hardware/gpio.h"

// confing encoders Pins
#define EncPinMlA 14
#define EncPinMlB 15
#define EncPinMrA 16
#define EncPinMrB 17

void encoder_setup();
void enc_mr_a_cb();
void enc_mr_b_cb();
void enc_ml_a_cb();
void enc_ml_b_cb();
void enc_measure_vel(int *mr_vel, int *ml_vel);

#endif