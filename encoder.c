#include "encoder.h"

// variables
static volatile int mr_pos = 0, ml_pos = 0;

void encoder_setup()
{
    encoder_setup_pin(EncPinMrA);
    encoder_setup_pin(EncPinMrB);
    encoder_setup_pin(EncPinMlA);
    encoder_setup_pin(EncPinMlB);

    gpio_set_irq_callback(EncPinMrA, &enc_mr_a_cb); 
    gpio_set_irq_callback(EncPinMrB, &enc_mr_b_cb);
    gpio_set_irq_callback(EncPinMlA, &enc_ml_a_cb);
    gpio_set_irq_callback(EncPinMlB, &enc_ml_b_cb);
}

void encoder_setup_pin(int pin){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void enc_mr_a_cb(){
    if (gpio_read(EncPinMrA)!=gpio_read(EncPinMrB)) { mr_pos++;}
    else { mr_pos--;}
}

void enc_mr_b_cb(){
    if (gpio_read(EncPinMrA)==gpio_read(EncPinMrB)) { mr_pos++;}
    else { mr_pos--;}
}

void enc_ml_a_cb(){
    if (gpio_read(EncPinMlA)==gpio_read(EncPinMlB)) { ml_pos++;} // fordward
    else { ml_pos--;} // backward
}

void enc_ml_b_cb(){
    if (gpio_read(EncPinMlA)!=gpio_read(EncPinMlB)) { ml_pos++;}
    else { ml_pos--;}
}

void enc_measure_vel(int *mr_vel, int *ml_vel)
{
    *mr_vel = mr_pos;
    mr_pos = 0;

    *ml_vel = ml_pos;
    ml_pos = 0;
}