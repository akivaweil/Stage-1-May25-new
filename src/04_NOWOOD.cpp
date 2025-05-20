#include "NoWood.h"
#include "settings.h"
#include <Arduino.h> // For Serial
#include <FastAccelStepper.h>
#include "StateMachine.h" // For state transitions

//* ************************************************************************
//* ***************************** NO WOOD ********************************
//* ************************************************************************
// This file contains the definitions for the 'no wood' state functions. 

void enterNoWoodState() {
  Serial.println("ENTERING NO_WOOD STATE");
  Serial.println("NO_WOOD: Returning Cut Motor to home...");
  if (cutMotorStepper) {
    cutMotorStepper->setSpeedInHz(CUT_MOTOR_RETURN_SPEED);
    cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION);
    cutMotorStepper->moveTo(0);
  }
  Serial.println("NO_WOOD: Returning Position Motor to home...");
  if (positionMotorStepper) {
    positionMotorStepper->setSpeedInHz(POSITION_MOTOR_RETURN_SPEED);
    positionMotorStepper->setAcceleration(POSITION_MOTOR_ACCELERATION);
    positionMotorStepper->moveTo(0);
  }
}

void runNoWoodState() {
  bool cutMotorAtHome = (cutMotorStepper) ? !cutMotorStepper->isRunning() && cutMotorStepper->getCurrentPosition() == 0 : true;
  bool positionMotorAtHome = (positionMotorStepper) ? !positionMotorStepper->isRunning() && positionMotorStepper->getCurrentPosition() == 0 : true;

  if (cutMotorAtHome && positionMotorAtHome) {
    if ((!cutMotorStepper || cutMotorStepper->getCurrentPosition() == 0) && 
        (!positionMotorStepper || positionMotorStepper->getCurrentPosition() == 0)) {
      Serial.println("NO_WOOD: Both motors returned home. Transitioning to IDLE.");
      transitionToState(IDLE);
    }
  }
} 