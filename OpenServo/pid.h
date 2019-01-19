#ifndef _OS_PID_H_
#define _OS_PID_H_

// Initialize the PID algorithm module.
void pid_init(int16_t position);

// Initialize the PID related register values.
int16_t pid_position_to_pwm(int16_t position);

#endif // _OS_PID_H_
