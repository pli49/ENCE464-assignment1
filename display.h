/*
 * display.h
 *
 * Header for display.c
 *
 * (Partially sourced from ENCE361 2020 fitnesswedgroup10, bsc42 - Bradley Scott, dat64, jna40 - Jon Narciso)
 * 
 * T3 Project Group 6 2021
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


/**
 * @function        OLEDPrintf.
 * @brief           Print to Tiva OLED display.
 * @param charLine  Line of the display to print on.
 * @param format    Format string to be printed.
 * @param ...       va_list of values to insert into format string.
*/
void OLEDPrintf (uint8_t charLine, char *format, ...);


/**
 * @function        oledPrintStatus.
 * @brief           Print status information to Tiva OLED display.
*/
void oledPrintStatus(void);


/**
 * @function            displayTask.
 * @brief               displayTask to be scheduled by FreeRTOS, displays system status information on the OLED display.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void displayTask (void *pvParameters);


/**
 * @function    initDisplayTask.
 * @brief       Initialize display task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initDisplayTask(void);

#endif /* DISPLAY_H_ */
