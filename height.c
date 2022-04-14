/*
 * height.c
 *
 * Defines heli height related tasks
 *
 * T3 Project Group 6 2021
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "control.h"
#include "userInputs.h"
#include "pid.h"
#include "height.h"
#include "priorities.h"
#include "pwm.h"
#include "shared.h"

extern systemState systemStatus;

extern pid_struct main_rotor;

extern xQueueHandle g_ADCQueue;

extern xSemaphoreHandle g_StatusMutex;

static void heightTask (void *pvParameters) {

  //set up parameters
    portTickType ui16LastTime;
    uint32_t ui32ADCInput;
    uint32_t ui32PollDelay = 3;
    ui16LastTime = xTaskGetTickCount();

    while (1) {
        if (xQueueReceive(g_ADCQueue, &ui32ADCInput, 0) == pdPASS) {
            xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
            //Calculate current Altitude
            systemStatus.currentAlt = systemStatus.referenceAlt - ui32ADCInput;
        
            /* Calculate the current altitude percentage. 
            Overall calculation done here is (currentAlt * (4/5)) / 8, 
            so at minimum height this would be 0 / 8 = 0 and at maximum 
            height this would be 800 / 8 = 100, giving the range of 0-100% */
            systemStatus.currentAltPercent = (systemStatus.currentAlt * ((4 << 8) / 5)) >> 11;
            

            systemStatus.mainPWMDuty = pid(systemStatus.currentAltPercent, systemStatus.targetAlt, 0.01, &main_rotor);
            //change the main PWM duty cycle using the pid function

            xSemaphoreGive(g_StatusMutex);
        }
        vTaskDelayUntil(&ui16LastTime, ui32PollDelay / portTICK_RATE_MS);
        // Wait for the required amount of tick, ensure a constant execution frequency
    }
}
/* initialize the height task with set task priority and stack size*/
uint32_t initHeightTask (void) {

    if (xTaskCreate (heightTask, (const portCHAR *)"Height", 128, NULL, PRIORITY_HEIGHT_TASK, NULL) != pdTRUE) {
        return (1);
    }
    UARTprintf(" Height controls initialized \n");
    return (0);
}
