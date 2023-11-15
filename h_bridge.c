#include "h_bridge.h"

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
    // Set period of HBMaxPWM cycles (0 to 255 inclusive)
    pwm_set_wrap(pwm_gpio_to_slice_num(IN), HBMaxPWM);
}

// limit the pwm value to HBMaxPWM or -HBMaxPWM
int h_bridge_pwm_limit(int pwm)
{
    if (abs(pwm) > HBMaxPWM)
    {
        if (pwm < 0)
        {
            return -HBMaxPWM;
        }
        return HBMaxPWM;
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
        pwm_set_gpio_level(INB, 0);  
    }
    else if (pwm < 0)
    {
        pwm_set_gpio_level(INA, 0); 
        pwm_set_gpio_level(INB, abs(pwm));   
    }
    else
    {
        pwm_set_gpio_level(INA, 0); 
        pwm_set_gpio_level(INB, 0);
    }
}