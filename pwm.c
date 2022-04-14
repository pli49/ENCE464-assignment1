/*
 * pwm.c
 *
 * Definition of PWM tasks
 *
 * T3 Project Group 6 2021
 */

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/tm4c123gh6pm.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "pid.h"
#include "priorities.h"
#include "pwm.h"
#include "shared.h"

extern systemState systemStatus;

extern xSemaphoreHandle g_StatusMutex;

 /* --------------------------------------------
 *  Functions to initialise main rotor PWM tasks
 *  --------------------------------------------
 */

 // Initialise main rotor PWM
void initMainPWM(void) {

    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    // Calculate the PWM period corresponding to PWM_RATE_HZ.
    uint32_t ui32PwmPeriod =  SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32PwmPeriod);

    // Set the pulse width for PWM_START_PC % duty cycle.
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, ui32PwmPeriod * PWM_START_RATE_HZ / 100);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
}

// Main rotor PWM task
static void mainPWMTask(void* pvParameters) {
    portTickType ui16LastTime;
    uint32_t ui32PollingDelay = 25;
    uint32_t ui32PwmPeriod =  SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;
    ui16LastTime = xTaskGetTickCount();
    while (1) {
        // Enable output.
        if (systemStatus.state == TAKEOFF || systemStatus.state == FLYING || systemStatus.state == LANDING) {
            xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
            PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, ui32PwmPeriod * systemStatus.mainPWMDuty / 100);
            //Set the main PWM pulse width

            xSemaphoreGive(g_StatusMutex);
            PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
            //Set the state of the main PWM
        }
        vTaskDelayUntil(&ui16LastTime, ui32PollingDelay / portTICK_RATE_MS);
        // Wait for the required amount of tick, ensure a constant execution frequency
    }
}

// Start main rotor PWM task, print message to UART
uint32_t initMainPWMTask(void) {
    initMainPWM();

    if (xTaskCreate(mainPWMTask, (const portCHAR*)"Main PWM", 128, NULL, PRIORITY_MAIN_PWM_TASK, NULL) != pdTRUE) {
        return (1);
    }
    UARTprintf(" Main Rotor PWM initialized \n");
    return (0);
}

/* --------------------------------------------
*  Functions to initialise tail rotor PWM tasks
*  --------------------------------------------
*/

// Initialise tail rotor PWM
void initTailPWM(void) {

    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    // Calculate the PWM period corresponding to PWM_RATE_HZ.
    uint32_t ui32PwmPeriod =  SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32PwmPeriod);

    // Set the pulse width for PWM_START_PC % duty cycle.
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM, ui32PwmPeriod * PWM_START_RATE_HZ / 100);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}

// Tail rotor PWM task similiarly to the main PWM task
static void tailPWMTask(void* pvParameters) {
    portTickType ui16LastTime;
    uint32_t ui32PollingDelay = 25;
    uint32_t ui32PwmPeriod =  SysCtlClockGet() / PWM_DIVIDER / PWM_START_RATE_HZ;
    ui16LastTime = xTaskGetTickCount();
    while (1) {
        if (systemStatus.state == IDLE) { //If the state is IDLE
            PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
            //Turn off tail PWM
        }
        else { // if the state is not IDLE
            xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
            PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, ui32PwmPeriod * systemStatus.tailPWMDuty / 100);
            //Set the pulse width for tail PWM

            xSemaphoreGive(g_StatusMutex);
            PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
            //Set the PWM output state for the tail PWM
        }
        vTaskDelayUntil(&ui16LastTime, ui32PollingDelay / portTICK_RATE_MS);
        // Wait for the required amount of time to check back, ensure a constant execution frequency
    }
}

// Start tail rotor PWM task
uint32_t initTailPWMTask(void) {
    initTailPWM();

    if (xTaskCreate(tailPWMTask, (const portCHAR*)"Tail PWM", 128, NULL, PRIORITY_TAIL_PWM_TASK, NULL) != pdTRUE) {
        return (1);
    }
    UARTprintf(" Tail Rotor PWM initialized \n");
    return (0);
}
