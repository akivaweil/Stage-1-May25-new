#pragma once
#include <Arduino.h>

//* ************************************************************************
//* ************************* STATE MACHINE ******************************
//* ************************************************************************
// This file contains the declarations for the state machine.

typedef enum {
  HOMING, // Though initial homing sequence is blocking, this can be a state if re-homing is needed.
  IDLE,
  READY,        // Added: Represents a state where the machine is ready for a new cycle
  CUTTING,
  YES_WOOD,
  NO_WOOD,
  ERROR         // Added: Represents an error condition state
  // Add other states here
} MachineState;

// Define error codes that can be set
enum ErrorCode {
    NO_ERROR_EC, 
    CUT_MOTOR_HOME_ERROR_EC,
    POSITION_MOTOR_HOME_ERROR_EC,
    CUT_MOTOR_TIMEOUT_EC,
    POSITION_MOTOR_TIMEOUT_EC
    // Add other specific error codes as needed
};

extern MachineState currentState;
extern ErrorCode currentError; // Added: Global variable to store the current error code

void transitionToState(MachineState newState);
void runStateMachine();
void initializeStateMachine(); // To set the initial state if needed outside of homing sequence 

// --- Utility Functions ---
/**
 * @brief Non-blocking wait utility.
 * 
 * @param interval The duration to wait in milliseconds.
 * @param startTime Pointer to a variable holding the start time (millis()). 
 *                  This variable will be updated by the function on the first call for a new wait cycle.
 * @return true if the interval has passed, false otherwise.
 */
bool Wait(unsigned long interval, unsigned long* startTime); 