/* Defines OLED display tasks and helper functions
*
* (Partially sourced from ENCE361 2020 fitnesswedgroup10, bsc42 - Bradley Scott, dat64, jna40 - Jon Narciso)
*
* T3 Project Group 6 2021
*/

#include <stdarg.h>
#include <stdint.h>

#include "utils/ustdlib.h"
#include "display.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "FreeRTOS.h"
#include "task.h"

#include "priorities.h"
#include "shared.h"

extern systemState systemStatus;

/*OLEDPrintf function. The variable number of arguments using ... allows this function to print any desired output. */
void OLEDPrintf (uint8_t charLine, char *format, ...) {
    char string[17]; // Display fits 16 characters wide.

    va_list arg;
    va_start(arg, format);
    uvsnprintf (string, sizeof(string), format, arg);
    va_end(arg);

    OLEDStringDraw (string, 0, charLine);
}
/* set up what to print in the OLED
line 1: state of the system
line 2: altitude of the system
line 3: yaw of the system
line 4: if the system is on or not */
void oledPrintStatus(void) {
    OLEDPrintf(0, "State:%s                ", statesLookup[systemStatus.state]);
    OLEDPrintf(1, "Alt:%02d%% [%02d%%]", systemStatus.currentAltPercent, systemStatus.targetAlt);
    OLEDPrintf(2, "Yaw:%03d` [%03d`]", systemStatus.currentYawDegrees, systemStatus.targetYaw);
    OLEDPrintf(3, "System on: %s                ", systemStatus.system_on ? "YES" : "NO");
}
/* set up the display task.*/
static void displayTask (void *pvParameters) {
    portTickType ui16LastTime;
    uint32_t ui32PollDelay = 250;
    ui16LastTime = xTaskGetTickCount();

    while(1) {
        //print status on the OLED
        oledPrintStatus();
        // Wait for the required amount of tick, ensure a constant execution frequency
        vTaskDelayUntil(&ui16LastTime, ui32PollDelay / portTICK_RATE_MS);
    }
}
/* initialize the display task with set task priority and stack size*/
uint32_t initDisplayTask(void) {

    if(xTaskCreate(displayTask, (const portCHAR *)"Display", 128, NULL, PRIORITY_DISPLAY_TASK, NULL) != pdTRUE) {
        return(1);
    }

    UARTprintf(" UART initialized \n");
    return(0);
}
