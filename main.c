/*
Terminal1:
    # Find your serial [device name]:
    ls /dev/serial/by-id/*
    
    # Start micro_ros_agent:
    ros2 run micro_ros_agent micro_ros_agent serial --dev [device name]
  
  Terminal2:
    # visualice msgs
    rqt 
*/

#include "main.h"

const uint LED_PIN = 25;
unsigned int num_handles = 2;   //2 publisher
float roll, pitch;

int main()
{
    stdio_init_all();
    
    //turn on LED to indicate the start of program
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    mpu6050_setup();

    microros_setup();
    microros_add_pubs();
    microros_add_timers();
    microros_add_executor();
    
    while (true)
    {
        RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    }
    return 0;
}

void microros_setup(){
    rmw_uros_set_custom_transport(
		true,
		NULL,
		pico_serial_transport_open,
		pico_serial_transport_close,
		pico_serial_transport_write,
		pico_serial_transport_read
	);
   
    allocator = rcl_get_default_allocator();

    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 1000; 
    const uint8_t attempts = 120;

    RCCHECK(rmw_uros_ping_agent(timeout_ms, attempts));
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
    RCCHECK(rclc_node_init_default(&node, "pico_node", "", &support));
}

void microros_add_pubs(){
    RCCHECK(rclc_publisher_init_default(
        &roll_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
        "rpip/roll"));
    
    RCCHECK(rclc_publisher_init_default(
        &pitch_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
        "rpip/pitch"));
}

void microros_add_timers(){
    RCCHECK(rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(100),
        timer_callback));
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
    mpu6050_read_raw(&roll, &pitch);
    roll_msg.data = roll;
    pitch_msg.data = pitch;
    RCCHECK(rcl_publish(&roll_publisher, &roll_msg, NULL));
    RCCHECK(rcl_publish(&pitch_publisher, &pitch_msg, NULL));
}

void microros_add_executor(){
    RCCHECK(rclc_executor_init(&executor, &support.context, num_handles, &allocator));
    RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void error_loop(){
    while(1) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        sleep_ms(500);
   }
}
