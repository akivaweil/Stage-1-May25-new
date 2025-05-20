#include "StateMachine.h"
#include "settings.h"
#include "Homing.h"
#include "Cutting.h"
#include "YesWood.h"
#include "NoWood.h"
#include "Idle.h"
#include <Arduino.h>

//* ************************************************************************
//* ************************* STATE MACHINE ******************************
//* ************************************************************************
// This file contains the definitions for the state machine. 

MachineState currentState;
ErrorCode currentError = NO_ERROR_EC; // Definition for the global error code

// Helper function to convert MachineState enum to string for printing
const char* stateToString(MachineState state) {
  switch (state) {
    case HOMING: return "HOMING";
    case IDLE: return "IDLE";
    case CUTTING: return "CUTTING";
    case YES_WOOD: return "YES_WOOD";
    case NO_WOOD: return "NO_WOOD";
    default: return "UNKNOWN_STATE";
  }
}

void initializeStateMachine() {
    // Explicitly set initial state to HOMING
    currentState = HOMING;
    Serial.println("State Machine Initialized. Current state: HOMING");
}

void transitionToState(MachineState newState) {
    Serial.print("STATE TRANSITION: From ");
    Serial.print(stateToString(currentState));
    Serial.print(" -> ");
    Serial.println(stateToString(newState));

    currentState = newState;

    switch (currentState) {
        case IDLE:
            enterIdleState();
            break;
        case HOMING:
            // enterHomingState(); // If Homing becomes a re-enterable state managed by SM
            break;
        case CUTTING:
            performCutCycle(); // Call the cutting cycle function
            break;
        case YES_WOOD:
            // enterYesWoodState(); // Old function call
            showYesWoodIndicator(); // New function for YesWood state entry/indication
            break;
        case NO_WOOD:
            enterNoWoodState();
            break;
        // Add cases for other states and call their entry functions
        default:
            Serial.println("Transitioned to an unknown state!");
            break;
    }
}

void runStateMachine() {
    switch (currentState) {
        case IDLE:
            runIdleState();
            break;
        case HOMING:
            // runHomingLoop(); // If homing were an SM managed loop
            break;
        case CUTTING:
            // runCuttingState(); // Now handled by performCutCycle on entry
            break;
        case YES_WOOD:
            // runYesWoodState(); // Old function call
            handleYesWoodState(); // New function for YesWood state logic
            break;
        case NO_WOOD:
            runNoWoodState();
            break;
        // Add cases for other states
        default:
            // Serial.println("In an unknown state!"); // Can be too verbose
            break;
    }
}

// --- Utility Functions ---
bool Wait(unsigned long interval, unsigned long* startTime) {
  if (millis() - *startTime >= interval) {
    *startTime = millis(); // Reset start time for the next potential wait or just to update
    return true; // Interval has passed
  }
  return false; // Interval has not passed
} 