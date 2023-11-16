#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_FREQ 1000  // PWM frequency in Hz
#define PWM_RANGE 255  // PWM range (8-bit resolution)

#define RIGHT 21, 20 // IN1, IN2 Right motor
#define LEFT 19, 18  // IN1, IN2 Left motor

void h_bridge_setup(int IN1, int IN2, int IN3, int IN4);
void h_bridge_setup_pin(int IN);
int h_bridge_pwm_limit(int pwm);
void h_bridge_set_pwm(int INA, int INB, int pwm);
