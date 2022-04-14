/**
 * pid.c
 *
 * PID controller function
 *
 * T3 Project Group 6 2021
 */

#include "pid.h"

uint32_t pid(float input, float setpoint, float period, pid_struct *pidObj) {
    float error = setpoint - input;
    float P = pidObj->Kp * error;
    float dI = pidObj->Ki * error * period;
    float D = (pidObj->Kd/period)*(error - pidObj->prev_error);

    pidObj->prev_error = error;
    uint32_t control = P + pidObj->I + D;

// Clamp values for control to range 2 - 98% for PWM
    if (control > pidObj->output_max){
        control = pidObj->output_max;
    } else if (control < pidObj->output_min){
        control = pidObj->output_min;
    } else {
        pidObj->I += dI;
    }
    return control;
}
