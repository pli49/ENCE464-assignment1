/*
 * pid.h
 *
 * Header for pid.c
 *
 * T3 Project Group 6 2021
 */

#ifndef PID_H
#define PID_H

#include <stdint.h>

/**
 * @struct              pid_struct.
 * @brief               Contains all PID object's properties.
 * @brief               All properties of PID(Kp, Ki, Kd. ...) are stored in this structure.
 * 
 * @param Kp            Proportional gain.
 * @param Ki            Integral gain.
 * @param Kd            Derivative gain.
 * @param prev_error    Last calculted error value.
 * @param I             Integral 
 * @param output_min    Minimum output value (limits minimum motor duty cycle)
 * @param output_max    Maximum output value (limits maximum motor duty cycle)
*/
typedef struct _pid_struct{
    float Kp;
    float Ki;
    float Kd;
    float prev_error;
    float I;
    const uint8_t output_min;
    const uint8_t output_max;
} pid_struct;


/**
 * @function        pid
 * @brief           Calculate and output command for given PID structure.
 * @param input     Current error input.
 * @param setpoint  Target value.
 * @param period    Change in time since last update.
 * @param pidObj    Pointer to a PID structure.
 * @returns         uint32_t: Current output command from the PID controller.
*/
uint32_t pid(float input, float setpoint, float period, pid_struct *pidObj);

#endif /* PID_H_ */
