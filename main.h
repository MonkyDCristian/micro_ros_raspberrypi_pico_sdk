#include <rcl/rcl.h>
#include <rcl/error_handling.h>

#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/int16.h>

#include <rmw_microros/rmw_microros.h>

#include "pico/stdlib.h"

#include "pico_uart_transport.h"
#include "mpu6050_i2c.h"
#include "h_bridge.h"

rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;

rcl_publisher_t roll_publisher;
std_msgs__msg__Float32 roll_msg;

rcl_publisher_t pitch_publisher;
std_msgs__msg__Float32 pitch_msg;

rcl_subscription_t subscriber_motor_left;
std_msgs__msg__Int16 sub_motor_left_msg;

rcl_subscription_t subscriber_motor_right;
std_msgs__msg__Int16 sub_motor_right_msg;

rcl_timer_t timer;
rclc_executor_t executor;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void microros_setup();
void microros_add_pubs();
void microros_add_subs();
void sub_ml_callback(const void * msgin);
void sub_mr_callback(const void * msgin);
void microros_add_timers();
void timer_callback(rcl_timer_t *timer, int64_t last_call_time);
void microros_add_executor();
void error_loop();
