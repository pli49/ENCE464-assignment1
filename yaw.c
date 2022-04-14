/*
 * yaw.c
 *
 * Yaw interrupt functions, updates current yaw value in systemStatus struct
 *
 * T3 Project Group 6 2021
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "pid.h"
#include "priorities.h"
#include "shared.h"
#include "yaw.h"

extern systemState systemStatus;

extern pid_struct tail_rotor;

xSemaphoreHandle g_yawCalibrated;

extern xSemaphoreHandle g_StatusMutex;

// Decode yaw based on the method proposed by ENCE464 tutor Ben Mitchell
// THIS IS UNTESTED AS WE WERE ONLY DOING HEIGHT BEFORE 2021 LOCKDOWN
yawResult_t decodeYaw(bool channel_a, bool channel_b) {
    static bool a_prime = false;
    static bool b_prime = false;

    yawResult_t result = 0;

    if (b_prime ^ channel_a) {
        result = LEFT;
    }
    if (a_prime ^ channel_b) {
        result = RIGHT;
    }
    a_prime = channel_a;
    b_prime = channel_b;

    return result;
}

void initYawSensor(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Set GPIO input for yaw channels A and B (PB0 / PB1)
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    // Set GPIO input for yaw reference (PC4)
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPadConfigSet (GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

// Initialize yaw interrupts
void initYawInterrupt(void) {
    GPIOIntRegister(GPIO_PORTB_BASE, yawInterrupt);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    GPIOIntRegister(GPIO_PORTC_BASE, yawReferenceInterrupt);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_INT_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
}

void yawInterrupt(void) {
    bool a = GPIOIntStatus(GPIO_PORTB_BASE, GPIO_PIN_0);
    bool b = GPIOIntStatus(GPIO_PORTB_BASE, GPIO_PIN_1);
    systemStatus.currentYaw += decodeYaw(a, b);
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

void yawReferenceInterrupt(void) {
    xSemaphoreGiveFromISR(g_yawCalibrated, NULL);
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
}

static void yawTask (void *pvParameters) {

  //Set up parameters
    portTickType ui16LastTime;
    uint32_t ui32PollDelay = 10;
    ui16LastTime = xTaskGetTickCount();

    while(1){
        xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
        systemStatus.currentYawDegrees = ((systemStatus.currentYaw * ((45 << 8) / 56)) >> 8);
        //Calculate the current Yaw values in degrees
        systemStatus.tailPWMDuty = pid(systemStatus.currentYawDegrees, systemStatus.targetYaw, 0.01, &tail_rotor);
        //Calculate the duty cycle for the tail PWM
        xSemaphoreGive(g_StatusMutex);
    }
    vTaskDelayUntil(&ui16LastTime, ui32PollDelay / portTICK_RATE_MS);
    // Wait for the required amount of tick, ensure a constant execution frequency
}

/* initialize the Yaw task with set task priority and stack size*/
uint32_t initYawTask (void) {
    initYawSensor();
    initYawInterrupt();

    g_yawCalibrated = xSemaphoreCreateBinary();

    if (xTaskCreate (yawTask, (const portCHAR *)"Yaw", 128, NULL, PRIORITY_YAW_TASK, NULL) != pdTRUE) {
        return (1);
    }
    UARTprintf(" Yaw reader initialized \n");
    return (0);
}
