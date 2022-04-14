/*
 * height.h
 *
 * Header for height.c
 * 
 * T3 Project Group 6 2021
 */
 

#ifndef HEIGHT_H_
#define HEIGHT_H_


/**
 * @function            heightTask.
 * @brief               heightTask to be scheduled by FreeRTOS, calculates current altitude and sets main rotor PWM using pid controller.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void heightTask (void *pvParameters);


/**
 * @function    initHeightTask.
 * @brief       Initialize height task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initHeightTask (void);

#endif /* HEIGHT_H_ */
