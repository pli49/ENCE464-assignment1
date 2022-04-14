/*
 * shared.h
 *
 * Data that needs to be shared between modules
 *
 * T3 Project Group 6 2021
 */

#ifndef SHARED_H_
#define SHARED_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @enum            programState.
 * @brief           Enumerated flight modes, used in systemState to keep track of overall system state.
*/
typedef enum _programState {
    IDLE,
    CALIBRATE,
    TAKEOFF,
    FLYING,
    LANDING
} programState;


/**
 * @struct                  systemState.
 * @brief                   Contains system state variables.
 * @brief                   System state variables (state, system_on, altCalibrated, ...) are stored in this structure.
 * 
 * @param state             System state structure.     
 * @param system_on         System on flag (Left switch ON position).
 * @param yawCalibrated     Yaw calibrated flag.
 * @param referenceAlt      Reference raw altitude.
 * @param currentAlt        Current raw altitude.
 * @param currentAltPercent Percentage altitude.
 * @param currentYaw        Current yaw value.
 * @param targetYaw         Target yaw value.
 * @param currentYawDegrees Current yaw in degrees.
 * @param mainPWMDuty       mainPWMDuty value.
 * @param tailPWMDuty       tailPWMDuty value.
*/
typedef struct _systemState {
    programState state;
    bool system_on;
    bool yawCalibrated;
    uint32_t referenceAlt;
    int32_t currentAlt;
    uint32_t targetAlt;
    int32_t currentAltPercent;
    uint32_t currentYaw;
    uint32_t targetYaw;
    uint32_t currentYawDegrees;
    uint8_t mainPWMDuty;
    uint8_t tailPWMDuty;
} systemState;

// Not really needed, but allows lookup of systemStatus.state to display current system state as a string instead of enum value
static const char *statesLookup[6] = {"IDLE", "CALIBRATE", "TAKEOFF", "FLYING", "LANDING", "LANDED"};

#endif /* SHARED_H_ */
