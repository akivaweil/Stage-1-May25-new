#include "YesWood.h"
#include "settings.h"
#include <Arduino.h> // For Serial
#include <FastAccelStepper.h>
#include "StateMachine.h" // For state transitions

//* ************************************************************************
//* **************************** YES WOOD ********************************
//* ************************************************************************
// This file contains the definitions for the 'wood present' state functions. 

void enterYesWoodState() {
  Serial.println("ENTERING YES_WOOD STATE");
  Serial.println("YES_WOOD: Returning Cut Motor to home...");
  if (cutMotorStepper) {
    cutMotorStepper->setSpeedInHz(CUT_MOTOR_RETURN_SPEED);
    cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION);
    cutMotorStepper->moveTo(0);
  }
  Serial.println("YES_WOOD: Returning Position Motor to home...");
  if (positionMotorStepper) {
    positionMotorStepper->setSpeedInHz(POSITION_MOTOR_RETURN_SPEED);
    positionMotorStepper->setAcceleration(POSITION_MOTOR_ACCELERATION);
    positionMotorStepper->moveTo(0);
  }
}

void runYesWoodState() {
  bool cutMotorAtHome = (cutMotorStepper) ? !cutMotorStepper->isRunning() && cutMotorStepper->getCurrentPosition() == 0 : true;
  bool positionMotorAtHome = (positionMotorStepper) ? !positionMotorStepper->isRunning() && positionMotorStepper->getCurrentPosition() == 0 : true;

  if (cutMotorAtHome && positionMotorAtHome) {
    // Check one last time after motors stop if they are indeed at 0
    // This helps if isRunning becomes false slightly before position is exactly 0.
    // However, FastAccelStepper's moveTo should ensure it reaches the target.
    // For robustness, an explicit check of getCurrentPosition() == 0 is good.
    if ((!cutMotorStepper || cutMotorStepper->getCurrentPosition() == 0) && 
        (!positionMotorStepper || positionMotorStepper->getCurrentPosition() == 0)) {
        Serial.println("YES_WOOD: Both motors returned home. Transitioning to IDLE.");
        transitionToState(IDLE);
    }
  }
} 