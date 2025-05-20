#pragma once
#include <Arduino.h>

//* ************************************************************************
//* ************************* STATE MACHINE ******************************
//* ************************************************************************
// This file contains the declarations for the state machine.

typedef enum {
  HOMING, // Though initial homing sequence is blocking, this can be a state if re-homing is needed.
  IDLE,
  CUTTING,
  YES_WOOD,
  NO_WOOD
  // Add other states here
} MachineState;

extern MachineState currentState;

void transitionToState(MachineState newState);
void runStateMachine();
void initializeStateMachine(); // To set the initial state if needed outside of homing sequence 