/*
 * userInputs.c
 *
 * Definition of input tasks on the Tiva / Orbit Boosterpack
 *
 * T3 Project Group 6 2021
 */

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "utils/uartstdio.h"

#include "priorities.h"
#include "shared.h"
#include "userInputs.h"
#include "uart.h"

extern systemState systemStatus;

xQueueHandle g_InputQueue;

extern xSemaphoreHandle g_UARTMutex;
xSemaphoreHandle g_PrintSemaphore;

/*  Initialise structs for each input object.
    Implementing each input as an 'object' using a struct means that new input methods
    can be added by creating a new instance of userInput_t and initialising as required.
*/

// LEFT button (Tiva Launchpad)
static userInput_t g_left_button = {
    .is_button = true,
    .is_portf = true,
    .status = false,
    .current_button_state = false,
    .input_event = NONE,
    .debounce_counter = 0,
    .periph = LEFT_BUT_PERIPH,
    .port_base = LEFT_BUT_PORT_BASE,
    .pin = LEFT_BUT_PIN,
    .gpio_strength = LEFT_BUT_GPIO_STRENGTH,
    .gpio_pin_type = LEFT_BUT_GPIO_PIN_TYPE
};

// RIGHT button (Tiva Launchpad)
static userInput_t g_right_button = {
    .is_button = true,
    .is_portf = true,
    .is_right_button = true,
    .status = false,
    .current_button_state = false,
    .input_event = NONE,
    .debounce_counter = 0,
    .periph = RIGHT_BUT_PERIPH,
    .port_base = RIGHT_BUT_PORT_BASE,
    .pin = RIGHT_BUT_PIN,
    .gpio_strength = RIGHT_BUT_GPIO_STRENGTH,
    .gpio_pin_type = RIGHT_BUT_GPIO_PIN_TYPE
};

// UP button (Orbit Boosterpack)
static userInput_t g_up_button = {
    .is_button = true,
    .status = false,
    .current_button_state = false,
    .input_event = NONE,
    .debounce_counter = 0,
    .periph = UP_BUT_PERIPH,
    .port_base = UP_BUT_PORT_BASE,
    .pin = UP_BUT_PIN,
    .gpio_strength = UP_BUT_GPIO_STRENGTH,
    .gpio_pin_type = UP_BUT_GPIO_PIN_TYPE
};

// DOWN button (Orbit Boosterpack)
static userInput_t g_down_button = {
    .is_button = true,
    .status = false,
    .current_button_state = false,
    .input_event = NONE,
    .debounce_counter = 0,
    .periph = DOWN_BUT_PERIPH,
    .port_base = DOWN_BUT_PORT_BASE,
    .pin = DOWN_BUT_PIN,
    .gpio_strength = DOWN_BUT_GPIO_STRENGTH,
    .gpio_pin_type = DOWN_BUT_GPIO_PIN_TYPE
};

// RIGHT switch (Orbit Boosterpack)
static userInput_t g_right_switch = {
    .periph = RIGHT_SW_PERIPH,
    .port_base = RIGHT_SW_PORT_BASE,
    .pin = RIGHT_SW_PIN,
    .gpio_strength = RIGHT_SW_GPIO_STRENGTH,
    .gpio_pin_type = RIGHT_SW_GPIO_PIN_TYPE
};

void initInputObj(userInput_t *inputObj) {
    SysCtlPeripheralEnable(inputObj->periph);

    // Note that PF0 is one of a handful of GPIO pins that need to be
    // "unlocked" before they can be reconfigured.  This also requires
    //      #include "inc/tm4c123gh6pm.h"
    if (inputObj->is_right_button) {
        GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
        GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
        GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    }

    GPIOPinTypeGPIOInput(inputObj->port_base, inputObj->pin);
    GPIOPadConfigSet(inputObj->port_base, inputObj->pin, inputObj->gpio_strength, inputObj->gpio_pin_type);

    if (!(inputObj->is_button)) {
        systemStatus.system_on = GPIOPinRead(inputObj->port_base, inputObj->pin) != 0;
    }
}

// Check state of button object, return true if button pressed
bool checkButtonState(userInput_t *inputObj) {
    bool state = false;
    // Set button pressed state to true if button PUSHED event occurs
    if((inputObj->was_pressed) && (inputObj->input_event) == PUSHED) state = true;
    inputObj->was_pressed = false;
    return state;
}

void updateInputObj(userInput_t *inputObj) {
    // Read GPIO pin for specified input object and store in object->status
    inputObj->status = GPIOPinRead(inputObj->port_base, inputObj->pin);

    // Logic for if inputObj is a button as updateInputObj handles both button and switch inputs
    if (inputObj->is_button) {

        // If new button status does not match stored current button state, proceed with update to new state
        if (inputObj->status != inputObj->current_button_state) {
            // Increment the debounce counter
            inputObj->debounce_counter++;

            // Ignore new button presses if debouncing threshold <= 10
            if(inputObj->debounce_counter >= 10) {
                // Update stored button state to new button status
                inputObj->current_button_state = inputObj->status;
                // Reset debounce counter
                inputObj->debounce_counter = 0;

                // Determine what button event has occurred for left and right buttons on portf of the Tiva
                if(inputObj->is_portf) {
                    // Set default button pressed status to NONE (no change)
                    inputObj->input_event = NONE;

                    // Left / Right buttons are active low (set inputObj->pressed to PUSHED if current state == 0, RELEASED if current state == 1)
                    inputObj->input_event = inputObj->current_button_state ? RELEASED : PUSHED;
                }
                else {
                // Determine what button event has occurred for up and down buttons on portd / porte of the Tiva
                    inputObj->input_event = NONE;

                    // Up / Down buttons are active high (set inputObj->pressed to PUSHED if current state == 1, RELEASED if current state == 0)
                    inputObj->input_event = inputObj->current_button_state ? PUSHED : RELEASED;
                }
                // Set flag for checkButtonState, indicating that button was pressed
                inputObj->was_pressed = true;
            }
        } else {
            // Set pressed status to NONE if current button state == new button state
            inputObj->input_event = NONE;
        }
    }
    else if (!(inputObj->is_button)) {
        systemStatus.system_on = GPIOPinRead(inputObj->port_base, inputObj->pin) != 0;
    }
}

// Update all user input devices (buttons and switch)
void updateInputs(void) {
    updateInputObj(&g_left_button);
    updateInputObj(&g_right_button);
    updateInputObj(&g_up_button);
    updateInputObj(&g_down_button);
    updateInputObj(&g_right_switch);
}

void initInputs(void) {
    initInputObj(&g_left_button);
    initInputObj(&g_right_button);
    initInputObj(&g_up_button);
    initInputObj(&g_down_button);
    initInputObj(&g_right_switch);
}

static void inputsTask (void *pvParameters) {
    portTickType ui16LastTime;
    uint32_t ui32PollDelay = 10;
    uint8_t ui8InputMessage;

    ui16LastTime = xTaskGetTickCount();

    while(1)
    {
        updateInputs();

        bool leftButtonPressed = checkButtonState(&g_left_button);
        bool rightButtonPressed = checkButtonState(&g_right_button);
        bool upButtonPressed = checkButtonState(&g_up_button);
        bool downButtonPressed = checkButtonState(&g_down_button);

        if(systemStatus.state == IDLE || systemStatus.state == FLYING) {
            if(leftButtonPressed) {
                ui8InputMessage = LEFT_BUTTON;

                // Guard UART from concurrent access.
                xSemaphoreTake(g_UARTMutex, portMAX_DELAY);
                UARTprintf("Left Button is pressed.\n");
                xSemaphoreGive(g_UARTMutex);
            }
            else if(rightButtonPressed) {
                ui8InputMessage = RIGHT_BUTTON;

                // Guard UART from concurrent access.
                xSemaphoreTake(g_UARTMutex, portMAX_DELAY);
                UARTprintf("Right Button is pressed.\n");
                xSemaphoreGive(g_UARTMutex);
            }
            else if(upButtonPressed) {
                ui8InputMessage = UP_BUTTON;

                // Guard UART from concurrent access.
                xSemaphoreTake(g_UARTMutex, portMAX_DELAY);
                UARTprintf("Up Button is pressed.\n");
                xSemaphoreGive(g_UARTMutex);
            }
            else if(downButtonPressed) {
                ui8InputMessage = DOWN_BUTTON;

                // Guard UART from concurrent access.
                xSemaphoreTake(g_UARTMutex, portMAX_DELAY);
                UARTprintf("Down Button is pressed.\n");
                xSemaphoreGive(g_UARTMutex);
            }
            else {
                ui8InputMessage = NO_INPUT;
            }
        }
        // Add the input message to the userInputADCQueue.
        if(xQueueSend(g_InputQueue, &ui8InputMessage, portMAX_DELAY) != pdPASS) {
            // Error. The queue should never be full. If so print the error message on UART and wait forever.
            xSemaphoreTake (g_UARTMutex, portMAX_DELAY);
            UARTprintf("\nInputs queue full.\n");
            xSemaphoreGive (g_UARTMutex);
            while(1){
            }
        }
        // Wait for the required amount of tick, ensure a constant execution frequency
        vTaskDelayUntil(&ui16LastTime, ui32PollDelay / portTICK_RATE_MS);
    }
}
/* initialize the input task with set periority task and stack size*/
uint32_t initInputTask (void) {
    initInputs();

    g_InputQueue = xQueueCreate(10, sizeof(uint8_t));

    if (xTaskCreate (inputsTask, (const portCHAR *)"UserInputs", 128, NULL, PRIORITY_INPUT_TASK, NULL) != pdTRUE) {
        return (1);
    }
    UARTprintf(" User inputs initialized \n");
    return (0);
}
