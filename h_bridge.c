#include "h_bridge.h"

const uint MAX_PWM_LEVEL = 65535;

void h_bridge_setup(int IN1, int IN2, int IN3, int IN4)
{
    h_bridge_setup_pin(IN1);
    h_bridge_setup_pin(IN2);
    h_bridge_setup_pin(IN3);
    h_bridge_setup_pin(IN4);
}

void h_bridge_setup_pin(int IN)
{
    // initialize PWM
    gpio_set_function(IN, GPIO_FUNC_PWM);
    pwm_set_wrap(pwm_gpio_to_slice_num(IN), PWM_RANGE);
    pwm_set_clkdiv(pwm_gpio_to_slice_num(IN), 16.0f);
    pwm_set_enabled(pwm_gpio_to_slice_num(IN), true);
}

// limit the pwm value to PWM_RANGE or -PWM_RANGE
int h_bridge_pwm_limit(int pwm)
{
    if (abs(pwm) > PWM_RANGE)
    {
        if (pwm < 0)
        {
            return -PWM_RANGE;
        }
        return PWM_RANGE;
    }
    return pwm;
}

// Set the pines INA or INB pwm value and left the other in zero depending of the sign of the pwm
void h_bridge_set_pwm(int INA, int INB, int pwm)
{
    pwm = h_bridge_pwm_limit(pwm);

    if (pwm > 0)
    {
        pwm_set_gpio_level(INA, pwm); 
        sleep_ms(10);  // Adjust the delay as needed
        pwm_set_gpio_level(INB, 0);  
        sleep_ms(10);  // Adjust the delay as needed
    }
    else if (pwm < 0)
    {
        pwm_set_gpio_level(INA, 0);
        sleep_ms(10);  // Adjust the delay as needed 
        pwm_set_gpio_level(INB, abs(pwm)); 
        sleep_ms(10);  // Adjust the delay as needed  
    }
    else
    {
        pwm_set_gpio_level(INA, 0); 
        sleep_ms(10);  // Adjust the delay as needed
        pwm_set_gpio_level(INB, 0);
        sleep_ms(10);  // Adjust the delay as needed
    }
}