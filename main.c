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
unsigned int num_handles = 1;   //2 sub + 1 timer
float roll, pitch;

int main()
{   
    //turn on LED to indicate the start of program
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    //h_bridge_setup(RIGHT, LEFT);
   
    mpu6050_setup();

    microros_setup();
    microros_add_pubs();
    //microros_add_subs();
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

// ---- MICROROS PUB -----
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

// ---- MICROROS SUB -----
/*
void microros_add_subs(){
  RCCHECK(rclc_subscription_init_default( // create subscriber
    &subscriber_motor_left, 
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int16), 
    "rpip/motor_right_sub"));

  RCCHECK(rclc_subscription_init_default( // create subscriber
    &subscriber_motor_right, 
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int16), 
    "rpip/motor_left_sub"));
}

void sub_ml_callback(const void * msgin){
  const std_msgs__msg__Int16 *msg = (const std_msgs__msg__Int16 *) msgin;
  h_bridge_set_pwm(RIGHT, msg->data);
}

void sub_mr_callback(const void * msgin){
  const std_msgs__msg__Int16 *msg = (const std_msgs__msg__Int16 *) msgin;
  h_bridge_set_pwm(LEFT, msg->data);
}
*/

// ---- MICROROS TIMERS -----
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
    //RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_motor_left, &sub_motor_left_msg, &sub_mr_callback, ON_NEW_DATA));
    //RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_motor_right, &sub_motor_right_msg, &sub_ml_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void error_loop(){
    while(1) {
        gpio_put(LED_PIN, !gpio_get(LED_PIN));
        sleep_ms(500);
   }
}
