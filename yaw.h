/*
 * yaw.h
 *
 * Header for yaw.c
 *
 * T3 Project Group 6 2021
 */

#ifndef YAW_H_
#define YAW_H_


/**
 * @enum            yawResult.
 * @brief           Enum for yaw values.
*/
typedef enum yawResult {
    LEFT = -1,
    RIGHT = 1
} yawResult_t;


/**
 * @function        decodeYaw.
 * @brief           Decodes quadrature rotations into left of right rotation.
 * @param channel_a Input channel A from quadrature encoder.
 * @param channel_b Input channel B from quadrature encoder.
 * @returns         yawResult_t: -1 if rotated LEFT, 0 if no rotation, 1 if rotated RIGHT.
*/
yawResult_t decodeYaw(bool channel_a, bool channel_b);


/**
 * @function        initYawSensor.
 * @brief           Initialize yaw readings on Tiva ports PB0 and PB1 and yaw reference readings on PC4.
*/
void initYawSensor(void);


/**
 * @function        initYawInterrupt.
 * @brief           Initialize yaw interrupts.
*/
void initYawInterrupt(void);


/**
 * @function        yawInterrupt.
 * @brief           Handle yaw interrupts, decoding yaw and modifying systemStatus.
*/
void yawInterrupt(void);


/**
 * @function        yawReferenceInterrupt.
 * @brief           Handle yaw reference interrupt when calibrating yaw, setting yawCalibrated semaphore flag.
*/
void yawReferenceInterrupt(void);


/**
 * @function            yawTask.
 * @brief               yawTask to be scheduled by FreeRTOS, calculates current yaw in degrees and sets tailPWMDuty.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void yawTask (void *pvParameters);


/**
 * @function    initYawTask.
 * @brief       Initialize yaw task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initYawTask (void);

#endif /* YAW_H_ */
