/*
 * adc.c
 *
 *  Definition of ADC tasks
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
#include "utils/uartstdio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "adc.h"
#include "display.h"
#include "circBufT.h"
#include "priorities.h"
#include "shared.h"
#include "uart.h"

#define BUF_SIZE 10

extern systemState systemStatus;

xQueueHandle g_ADCQueue;

extern xSemaphoreHandle g_UARTMutex;

bool bufferFull;

static circBuf_t g_inBuffer;

/*Calculate the reference altitude using data stored in the circular buffer when the buffer is full*/
void calibrateReferenceAlt(void) {
    if(bufferFull) {
        systemStatus.referenceAlt = (((g_inBuffer.sum << 1) + BUF_SIZE) >> 1) / BUF_SIZE;
    }
}

static void ADCTask(void *pvParameters) {
    portTickType ui16LastTime;
    uint8_t ui8bufferVals = 0;
    uint32_t ui32pollDelay = 3;

    ui16LastTime = xTaskGetTickCount();

    while(1) {
        ADCProcessorTrigger(ADC0_BASE, 3);

        if (bufferFull) {
            uint32_t ui32avgHGT = (((g_inBuffer.sum << 1) + BUF_SIZE) >> 1) / BUF_SIZE;  //Calculate the average height

            if(systemStatus.state == TAKEOFF || systemStatus.state == FLYING || systemStatus.state == LANDING) {
            // if the state of the system is in 'TAKEOFF', 'FLYING' or 'LANDING'

                if (xQueueSend(g_ADCQueue, (&ui32avgHGT), 0) != pdPASS) {
                    // if the queue is full
                    xSemaphoreTake (g_UARTMutex, portMAX_DELAY); // wait portMAX_DELAY to see if it becomes available to access the resource
                    UARTprintf("\nADC Queue full.\n");  // Print to the UART
                    xSemaphoreGive (g_UARTMutex);  //We have finished accessing the shared resource so can free the semaphore.
                }
            }
        }
        else if (!bufferFull) {  //if buffer is not full, incrament ui8bufferVals and check if buffer will be full for the next time
            ui8bufferVals++;
            if (ui8bufferVals == BUF_SIZE-1) bufferFull = true; //determine if the buffer is full
        }
        vTaskDelayUntil(&ui16LastTime, ui32pollDelay / portTICK_RATE_MS);
        // Wait for the required amount of tick, ensure a constant execution frequency
    }
}

void initADC (void) {
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

uint32_t initADCTask(void) {
    //Initialize ADC
    initADC();

    //Initialize ADC circular buffer
    initCircBuf (&g_inBuffer, BUF_SIZE);

    //Initialize ADCQueue
    g_ADCQueue = xQueueCreate(20, sizeof(uint32_t));

    // Create FreeRTOS task
    if(xTaskCreate(ADCTask, "ADC", 128, NULL, PRIORITY_ADC_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    UARTprintf(" ADC initialized \n");
    return(0);
}

void ADCIntHandler(void) {
    uint32_t ui32ulValue;

    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ui32ulValue);

    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ui32ulValue);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}
