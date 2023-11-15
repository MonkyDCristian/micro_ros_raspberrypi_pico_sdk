#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define PICO_I2C_SDA_PIN 0
#define PICO_I2C_SCL_PIN 1

void mpu6050_setup();
void mpu6050_reset();
void mpu6050_read_raw(float *roll, float *pitch);
void mpu6050_get_error();