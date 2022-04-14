/*
 * uart.h
 *
 * Header for uart.c.
 *
 * T3 Project Group 6 2021
 */

#ifndef UART_H_
#define UART_H_

/**
 * @function        configUART.
 * @brief           Configure UART for serial communications.
*/
void configUART (void);


/**
 * @function            uartTask.
 * @brief               uartTask to be scheduled by FreeRTOS, sends system status information over UART to PC.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void uartTask(void* pvParameters);

/**
 * @function    initUartTask.
 * @brief       Initialize UART task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initUartTask(void);

#endif /* UART_H_ */
