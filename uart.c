/*
 *  uart.c
 *
 *  Definition of UART tasks
 *	Sends helirig data over serial communications
 *
 *  T3 Project Group 6 2021
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "uart.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "priorities.h"
#include "shared.h"


extern systemState systemStatus;

xSemaphoreHandle g_UARTMutex;

void configUART (void) {
    //Enable GPIO for UART
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //Config GPIO pins
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //Set UART clock source
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //UART transmission settings
    UARTStdioConfig(0, 115200, 16000000);
}

/*print system information to through UART*/
void printStatus(void) {
    xSemaphoreTake(g_UARTMutex, portMAX_DELAY);
    UARTprintf("\r\nSystem state:\t%s\r\n", statesLookup[systemStatus.state]);
    UARTprintf("Altitude:\t%02d%% [%02d%%]\r\n", systemStatus.currentAltPercent, systemStatus.targetAlt);
    UARTprintf("Yaw:\t%03d%c [%03d%c]\r\n", systemStatus.currentYawDegrees, 176, systemStatus.targetYaw, 176);
    UARTprintf("Duty cycle: Main: %d%%, Tail: %d%%\r\n", systemStatus.mainPWMDuty, systemStatus.tailPWMDuty);
    UARTprintf("System on: %d\r\n", systemStatus.system_on);
    xSemaphoreGive(g_UARTMutex);
}

static void uartTask (void *pvParameters) {

  //set up some Parameters
    portTickType ui16LastTime;
    uint32_t ui32PollDelay = 250;
    ui16LastTime = xTaskGetTickCount();

    while(1) {
        printStatus(); //Call the print function to print system information for testing
        vTaskDelayUntil(&ui16LastTime, ui32PollDelay / portTICK_RATE_MS);
        // Wait for the required amount of tick, ensure a constant execution frequency
    }
}


/* initialize the UART task with set task priority and stack size*/
uint32_t initUartTask(void) {
    g_UARTMutex = xSemaphoreCreateMutex();

    if(xTaskCreate(uartTask, (const portCHAR *)"UART", 128, NULL, PRIORITY_UART_TASK, NULL) != pdTRUE) {
        return(1);
    }

    UARTprintf(" UART initialized \n");
    return(0);
}
