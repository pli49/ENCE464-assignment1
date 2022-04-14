/*
 * adc.h
 *
 * Header for adc.c
 *
 * T3 Project Group 6 2021
 */

#ifndef ADC_H_
#define ADC_H_


/**
 * @function        calibrateReferenceAlt.
 * @brief           If ADC buffer is full, calculate and store landed reference altitude.
*/
void calibrateReferenceAlt(void); 


/**
 * @function            ADCTask.
 * @brief               ADCTask to be scheduled by FreeRTOS, triggers ADC sampling and adds average reading to the ADCQueue.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void ADCTask(void *pvParameters);


/**
 * @function        initADC.
 * @brief           Initialize ADC on AIN9.
*/
void initADC (void);


/**
 * @function    initADCTask.
 * @brief       Initialize ADC task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initADCTask(void);


/**
 * @function        ADCIntHandler.
 * @brief           Interrupt handler for ADC sampling, writing sample values to circular buffer.
*/
void ADCIntHandler(void);

#endif /* ADC_H_ */
