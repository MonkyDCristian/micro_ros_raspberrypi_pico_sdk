/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * https://github.com/raspberrypi/pico-examples/tree/master/i2c/mpu6050_i2c
 * Solution to zero output: https://forums.raspberrypi.com/viewtopic.php?t=354765
 * similar proyect: https://github.com/danjperron/StairPersonDetector
 * filter algorhim: https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
 */

#include "mpu6050_i2c.h"

/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/

// By default these devices  are on bus address 0x68
static int mpu_addr = 0x68;
int16_t accel[3];
float gyro[3];
float accAngleX, accAngleY;
float elapsedTime, currentTime, previousTime;

void mpu6050_setup(){
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_I2C_SDA_PIN);
    gpio_pull_up(PICO_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_I2C_SDA_PIN, PICO_I2C_SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();
}

void mpu6050_reset(){
    // Two byte reset. First byte register, second byte data: https://forums.raspberrypi.com/viewtopic.php?t=354765
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[2] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, mpu_addr, buf, 2, false);
}

void mpu6050_read_raw(float *roll, float *pitch){
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes. 0 > X, 1 > Y, 2 > Z
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, mpu_addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, mpu_addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) ; // X-Y-Z axis value
    }

    // Calculating Roll and Pitch angles from the accelerometer data
    accAngleX = (atan(accel[1] / sqrt(pow(accel[0], 2) + pow(accel[2], 2))) * 180 / M_PI);
    accAngleY = (atan(-1 * accel[0] / sqrt(pow(accel[1], 2) + pow(accel[2], 2))) * 180 / M_PI);

    // getting elapsed time
    previousTime = currentTime;           // Previous time is stored before the actual time read
    currentTime = time_us_64() / 1000000; // Current time actual time read
    elapsedTime = (float)(currentTime - previousTime);
    
    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, mpu_addr, &val, 1, true);
    i2c_read_blocking(i2c_default, mpu_addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
    }

    // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
    // Complementary filter - combine acceleromter and gyro angle values
    *roll = 0.96 * (*roll + gyro[0] * elapsedTime) + 0.04 * accAngleX;
    *pitch = 0.96 * (*pitch + gyro[1] * elapsedTime) + 0.04 * accAngleY;
}

