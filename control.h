/*
 * control.h
 *
 * Header for control.c
 *
 * T3 Project Group 6 2021
 */

#ifndef CONTROL_H_
#define CONTROL_H_


/**
 * @function            controlTask.
 * @brief               controlTask to be scheduled by FreeRTOS, controls system behaviours based on user input, ADC and yaw readings.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void controlTask (void *pvParameters);


/**
 * @function    initControlTask.
 * @brief       Initialize control task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initControlTask (void);

#endif /* CONTROL_H_ */
