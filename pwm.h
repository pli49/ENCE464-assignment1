/*
 * pwm.h
 *
 * Header for pwm.c
 *
 * T3 Project Group 6 2021
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define PWM_START_RATE_HZ  200
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_2
#define PWM_DIVIDER        2

//pwm queue
#define PWM_QUEUE_SIZE 10
#define DUTY_ITEM_SIZE sizeof(uint16_t)

 //  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE			PWM0_BASE
#define PWM_MAIN_GEN			PWM_GEN_3
#define PWM_MAIN_OUTNUM			PWM_OUT_7
#define PWM_MAIN_OUTBIT			PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM		SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO	SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE		GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG	GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN		GPIO_PIN_5

//  ---Tail Rotor PWM: PF1, J3-10
#define PWM_TAIL_BASE			PWM1_BASE
#define PWM_TAIL_GEN			PWM_GEN_2
#define PWM_TAIL_OUTNUM			PWM_OUT_5
#define PWM_TAIL_OUTBIT			PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM		SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO	SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE		GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG	GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN		GPIO_PIN_1


/**
 * @function        initMainPWM.
 * @brief           Initialize PWM for main rotor on pin PC5.
*/
void initMainPWM(void);


/**
 * @function            mainPWMTask.
 * @brief               mainPWMTask to be scheduled by FreeRTOS, enables PWM output depending on system state and updates main rotor PWM duty.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void mainPWMTask(void* pvParameters);


/**
 * @function    initMainPWMTask.
 * @brief       Initialize main rotor PWM task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initMainPWMTask(void);


/**
 * @function        initTailPWM.
 * @brief           Initialize PWM for tail rotor on pin PF1.
*/
void initTailPWM(void);


/**
 * @function            tailPWMTask.
 * @brief               tailPWMTask to be scheduled by FreeRTOS, enables PWM output depending on system state and updates tail rotor PWM duty.
 * @param pvParameters  Pointer to task parameters (NULL).
*/
static void tailPWMTask(void* pvParameters);


/**
 * @function    initTailPWMTask.
 * @brief       Initialize tail rotor PWM task.
 * @returns     uint32_t: 1 if xTaskCreate fails, else 0.   
*/
uint32_t initTailPWMTask(void);

#endif /* PWM_H_ */
