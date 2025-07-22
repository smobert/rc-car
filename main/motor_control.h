#include <stdbool.h>
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

void motor_control_init(void);
void set_motor_direction(bool left_forward, bool right_forward);
void stop_motors(void);
void set_motor_speed(int channel, int percent);

#endif

