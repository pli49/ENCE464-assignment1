/*
 * userInputs.h
 *
 * Header for userInputs.c
 *
 * T3 Project Group 6 2021
 */

#ifndef USERINPUTS_H_
#define USERINPUTS_H_


/**
 * @enum            input_events.
 * @brief           Categorize input event types.
*/
typedef enum input_events {
    RELEASED = 0,
    PUSHED,
    SWITCH_ON,
    SWITCH_OFF, 
    NONE
} input_event_t;


/**
 * @enum            inputDevice_ID.
 * @brief           For adding unique input event identifiers to the queue.
*/
typedef enum inputDevice_ID {
    NO_INPUT = 0, 
    LEFT_BUTTON,
    RIGHT_BUTTON,
    UP_BUTTON,
    DOWN_BUTTON
} input_device_t;


/**
 * @struct                      userInput_t.
 * @brief                       Contains all input object's properties.
 * @brief                       All properties of an input object (is_button, is_right_button, is_portf, ...) are stored in this structure.
 * 
 * @param is_button             Object is button.     
 * @param is_right_button       Object is right_button.
 * @param is_portf              Object is on Tiva PORTF.
 * @param status                Object status.
 * @param current_button_state  Current button state (LOW / HIGH).
 * @param was_pressed           Button was pressed flag.
 * @param input_event           Instance of input_events structure, storing event type.
 * @param debounce_counter      Counter for debouncing input object.
 * 
 * Params for SysCtl / GPIO initialization and configuration
 * @param periph                Input object interface peripheral
 * @param port_base             Input object port address
 * @param pin                   Input object pin
 * @param gpio_strength         Input object drive strength
 * @param gpio_pin_type         Input object pin type
*/
typedef struct _userInput_t {
    bool is_button;
    bool is_right_button;
    bool is_portf;     // For Left / Right button inputs
    bool status;
    bool current_button_state;
    bool was_pressed;
    input_event_t input_event;
    uint8_t debounce_counter;
    uint32_t periph;
    uint32_t port_base;
    uint32_t pin;
    uint32_t gpio_strength;
    uint32_t gpio_pin_type;
} userInput_t;

// LEFT button (Tiva Launchpad)
#define LEFT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define LEFT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define LEFT_BUT_PIN  GPIO_PIN_4
#define LEFT_BUT_GPIO_STRENGTH GPIO_STRENGTH_2MA
#define LEFT_BUT_GPIO_PIN_TYPE GPIO_PIN_TYPE_STD_WPU

// RIGHT button (Tiva Launchpad)
#define RIGHT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define RIGHT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define RIGHT_BUT_PIN  GPIO_PIN_0
#define RIGHT_BUT_GPIO_STRENGTH GPIO_STRENGTH_2MA
#define RIGHT_BUT_GPIO_PIN_TYPE GPIO_PIN_TYPE_STD_WPU

// UP button (Orbit Boosterpack)
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0
#define UP_BUT_GPIO_STRENGTH GPIO_STRENGTH_2MA
#define UP_BUT_GPIO_PIN_TYPE GPIO_PIN_TYPE_STD_WPD

// DOWN button (Orbit Boosterpack)
#define DOWN_BUT_PERIPH  SYSCTL_PERIPH_GPIOD
#define DOWN_BUT_PORT_BASE  GPIO_PORTD_BASE
#define DOWN_BUT_PIN  GPIO_PIN_2
#define DOWN_BUT_GPIO_STRENGTH GPIO_STRENGTH_2MA
#define DOWN_BUT_GPIO_PIN_TYPE GPIO_PIN_TYPE_STD_WPD

// RIGHT switch (Orbit Boosterpack)
#define RIGHT_SW_PERIPH SYSCTL_PERIPH_GPIOA
#define RIGHT_SW_PORT_BASE GPIO_PORTA_BASE
#define RIGHT_SW_PIN GPIO_PIN_7
#define RIGHT_SW_GPIO_STRENGTH GPIO_STRENGTH_2MA
#define RIGHT_SW_GPIO_PIN_TYPE GPIO_PIN_TYPE_STD_WPD

extern xQueueHandle g_userInputADCQueue;


/**
 * @function        initInputObj.
 * @brief           Initialize input object, setting SysCtl and GPIO properties.
 * @param inputObj  Pointer to an input structure.

*/
void initInputObj(userInput_t *inputObj);


/**
 * @function        checkButtonState.
 * @brief           Check state of button object, returning true if the button was pushed.
 * @param inputObj  Pointer to an input structure (specific to buttons).
 * @returns         bool: 1 if button was pushed, else 0.
*/
bool checkButtonState(userInput_t *inputObj);


/**
 * @function        updateInputObj
 * @brief           Updates input object struct members and performs debouncing.
 * @param inputObj  Pointer to an input structure.
*/
void updateInputObj(userInput_t *inputObj);


/**
 * @function        updateInputs.
 * @brief           Update all input objects using updateInputObj.
*/
void updateInputs(void);


/**
 * @function        initInputs.
 * @brief           Initialize input objects using initInputObj.
*/
void initInputs(void);


/**
 * @function            inputsTask.
 * @brief               inputsTask to be scheduled by FreeRTOS, checks for user input and adds input events to the input queues.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void inputsTask (void *pvParameters);


/**
 * @function    initInputTask.
 * @brief       Initialize input task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initInputTask(void);

#endif /* USERINPUTS_H_ */
