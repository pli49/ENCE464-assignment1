/*
 * priorities.h
 *
 * Set task priorities
 *
 * T3 Project Group 6 2021
 */



#ifndef __PRIORITIES_H__
#define __PRIORITIES_H__

//*****************************************************************************
//
// The priorities of the various tasks.
//
//*****************************************************************************
#define PRIORITY_UART_TASK      2
#define PRIORITY_DISPLAY_TASK    2
#define PRIORITY_ADC_TASK        4
#define PRIORITY_INPUT_TASK      4
#define PRIORITY_CONTROL_TASK    3
#define PRIORITY_HEIGHT_TASK     4
#define PRIORITY_YAW_TASK        2
#define PRIORITY_MAIN_PWM_TASK   2
#define PRIORITY_TAIL_PWM_TASK   2

#endif // __PRIORITIES_H__
