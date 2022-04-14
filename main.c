/**
 * main.c
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
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "adc.h"
#include "circBufT.h"
#include "control.h"
#include "display.h"
#include "height.h"
#include "priorities.h"
#include "pwm.h"
#include "shared.h"
#include "uart.h"
#include "userInputs.h"
#include "yaw.h"

systemState systemStatus;

xSemaphoreHandle g_StatusMutex;

void initHeliState(systemState *systemStatus) {
    systemStatus->referenceAlt = 0;
    systemStatus->targetYaw = 0;
}

int main(void) {
  //initialise the required peripherals
    SysCtlClockSet (SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    configUART();

    OLEDInitialise();

    initHeliState(&systemStatus);

    g_StatusMutex = xSemaphoreCreateMutex();

    //create the required tasks by calling the initialise
    //function in each module

    if(initUartTask() != 0) {
        while(1);
    }

    if(initDisplayTask() != 0) {
        while(1);
    }

    if(initADCTask() != 0) {
        while(1);
    }

    if(initInputTask() != 0) {
        while(1);
    }

    if(initControlTask() != 0) {
        while (1);
    }

    if(initHeightTask() != 0) {
        while(1);
    }

    if(initMainPWMTask() != 0) {
        while (1);
    }

    if(initYawTask() != 0) {
        while(1);
    }

    if(initTailPWMTask() != 0) {
        while (1);
    }

//print to UART to show all tasks initialized
    UARTprintf("\n -----------------------------------------");
    UARTprintf("\n All tasks initialized, starting scheduler\n");
    UARTprintf(" -----------------------------------------\n\n");
//Enable interrupts to the processor
    IntMasterEnable();

//Start the scheduler to start executing our tasks
    vTaskStartScheduler();

    // Should never get here since the RTOS should never "exit".
    while(1);
}
