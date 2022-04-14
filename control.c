#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "utils/uartstdio.h"

#include "adc.h"
#include "control.h"
#include "shared.h"
#include "height.h"
#include "pid.h"
#include "priorities.h"
#include "uart.h"
#include "userInputs.h"
#include "yaw.h"

extern systemState systemStatus;

extern xQueueHandle g_InputQueue;

extern xSemaphoreHandle g_UARTMutex;
extern xSemaphoreHandle g_StatusMutex;
extern xSemaphoreHandle g_yawCalibrated;

/* set up the PID control variables and PWM range for main rotor*/
pid_struct main_rotor = {
    .Kp = 1,
    .Ki = 0.45,
    .Kd = 1,
    .prev_error = 0,
    .I = 0,
    .output_min = 2,
    .output_max = 98
};

/* set up the PID control variables and PWM range for tail rotor*/
pid_struct tail_rotor = {
    .Kp = 1,
    .Ki = 0.45,
    .Kd = 2,
    .prev_error = 0,
    .I = 0,
    .output_min = 2,
    .output_max = 98
};

static void controlTask (void *pvParameters) {
    portTickType ui16LastTime;
    uint32_t ui32PollDelay = 10;
    uint8_t ui8InputEvent;
    ui16LastTime = xTaskGetTickCount();

    while(1)
    {
        xQueueReceive(g_InputQueue, &ui8InputEvent, 0);
            switch(systemStatus.state) {
                case IDLE: //when current state is Idle state
                    xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
                    // Obtain mutex semaphore to update the shared resource values
                    systemStatus.mainPWMDuty = 0;
                    systemStatus.tailPWMDuty = 0;
                    xSemaphoreGive(g_StatusMutex);
                    // Give mutex semaphore after resource update has finished
                    if(systemStatus.system_on && ui8InputEvent == UP_BUTTON) {
                    //if the system is on and the up button is pushed
                        if(!systemStatus.yawCalibrated) {
                        //if the system yaw was not calibrated, go to calibrate state
                            systemStatus.state = CALIBRATE;
                        }
                        else {
                        //if the system state was calibrated, go to takeoff state. This is used if the heli has landed and the user wants to takeoff again as yaw will be already calibrated.
                            systemStatus.state = TAKEOFF;
                        }
                    }
                    break;
                case CALIBRATE:     //when current state is calibrate
                    if(systemStatus.system_on) {    //if the system is on
                        calibrateReferenceAlt();    //calculate the reference altitude
                        xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
                        systemStatus.tailPWMDuty = 20;
                        xSemaphoreGive(g_StatusMutex);
                        if (xSemaphoreTake(g_yawCalibrated) == pdTRUE) {
                        //if a reference point is just found
                            systemStatus.currentYaw = 0;  //immediately set the point to 0 as the reference
                            systemStatus.yawCalibrated = true;  //set yawCalibrated to true
                            systemStatus.state = TAKEOFF;   //change the state to takeoff
                        }
                        break;
                    }
                    else {
                        systemStatus.state = IDLE; // if the system is turned off, change the state to idle
                    }
                case TAKEOFF:  //when current state is take off
                    if(systemStatus.system_on) {
                        xSemaphoreTake(g_StatusMutex, portMAX_DELAY);

                        //set the target yaw and target altitude for the system
                        systemStatus.targetYaw = 0;
                        systemStatus.targetAlt = 10;
                        xSemaphoreGive(g_StatusMutex);
                        if (systemStatus.currentAltPercent >= 10) {
                          //when the current altitude is greater then 10
                            systemStatus.state = FLYING;  //change the system state to flying
                        }
                        break;
                    } else { //if the system is turned off, change the system state to landing
                        systemStatus.state = LANDING;
                    }
                case FLYING:  //when the state is flying
                    if(systemStatus.system_on) {
                        switch (ui8InputEvent) {
                          //take the input from different buttons
                          //and change the yaw and altitude of the system
                            xSemaphoreTake(g_StatusMutex, portMAX_DELAY);
                            case LEFT_BUTTON:
                                systemStatus.targetYaw -= 10;
                            break;
                            case RIGHT_BUTTON:
                                systemStatus.targetYaw += 10;
                            break;
                            case UP_BUTTON:
                                if(systemStatus.targetAlt < 100) {
                                    systemStatus.targetAlt += 10;
                                }
                            break;
                            case DOWN_BUTTON:
                                if(systemStatus.targetAlt > 0) {
                                    systemStatus.targetAlt -= 10;
                                }
                            break;
                            xSemaphoreGive(g_StatusMutex);
                        }
                        if(systemStatus.targetAlt < 10) {
                          //If the target altitude is changed by the user to be below 10, change the state to landing
                            systemStatus.targetAlt = 10;
                            systemStatus.state = LANDING;
                        }
                        break;
                    } else {
                        systemStatus.state = LANDING;
                    }
                case LANDING:
                    systemStatus.targetYaw = 0;
                    if(abs(systemStatus.currentYawDegrees - systemStatus.targetYaw) < 5) {
                        systemStatus.targetAlt = 0;
                        if(systemStatus.currentAltPercent < 1) {
                            systemStatus.state = IDLE;
                        }
                    }
                    break;
            }
        // Wait for the required amount of tick, ensure a constant execution frequency
        vTaskDelayUntil(&ui16LastTime, ui32PollDelay / portTICK_RATE_MS);
    }
}
/* initialize the control task with set task priority and stack size*/
uint32_t initControlTask (void) {

    if (xTaskCreate (controlTask, (const portCHAR *)"control", 128, NULL, PRIORITY_CONTROL_TASK, NULL) != pdTRUE) {
        return (1);
    }
    UARTprintf(" Control initialized \n");
    return (0);
}
