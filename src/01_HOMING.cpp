#include "Homing.h"
#include "settings.h"
#include <FastAccelStepper.h>
#include <Bounce2.h>
#include "StateMachine.h" // For transitioning to IDLE state
#include <Arduino.h> // For Serial

//* ************************************************************************
//* ****************************** HOMING ********************************
//* ************************************************************************
// This file contains the definitions for the homing state functions. 

void runHomingSequence() {
  Serial.println("==== HOMING SEQUENCE STARTED ====");
  
  //! Setup Homing Switches with Bounce2
  Bounce cutMotorSwitch = Bounce();
  Bounce positionMotorSwitch = Bounce();

  Serial.println("Setting up homing switches...");
  
  // Initialize the homing switches
  pinMode(CUT_MOTOR_HOMING_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode(POSITION_MOTOR_HOMING_SWITCH_PIN, INPUT_PULLDOWN);
  
  Serial.print("CUT_MOTOR_HOMING_SWITCH_PIN ("); 
  Serial.print(CUT_MOTOR_HOMING_SWITCH_PIN);
  Serial.print(") initial state: ");
  Serial.println(digitalRead(CUT_MOTOR_HOMING_SWITCH_PIN) ? "HIGH" : "LOW");
  
  Serial.print("POSITION_MOTOR_HOMING_SWITCH_PIN (");
  Serial.print(POSITION_MOTOR_HOMING_SWITCH_PIN);
  Serial.print(") initial state: ");
  Serial.println(digitalRead(POSITION_MOTOR_HOMING_SWITCH_PIN) ? "HIGH" : "LOW");

  cutMotorSwitch.attach(CUT_MOTOR_HOMING_SWITCH_PIN, INPUT_PULLDOWN);
  positionMotorSwitch.attach(POSITION_MOTOR_HOMING_SWITCH_PIN, INPUT_PULLDOWN);

  cutMotorSwitch.interval(5); // 10 ms debounce interval
  positionMotorSwitch.interval(5); // 10 ms debounce interval

  // Force an initial update of switch states
  cutMotorSwitch.update();
  positionMotorSwitch.update();
  
  Serial.print("Cut Motor Switch initial state: ");
  Serial.println(cutMotorSwitch.read() ? "HIGH" : "LOW");
  Serial.print("Position Motor Switch initial state: ");
  Serial.println(positionMotorSwitch.read() ? "HIGH" : "LOW");

  bool cutMotorHomed = false;
  bool positionMotorHomed = false;

  Serial.println("Starting cut motor homing...");
  if (cutMotorStepper) {
    cutMotorStepper->setSpeedInHz(CUT_MOTOR_HOMING_SPEED);
    cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION); 
    cutMotorStepper->move(-2000000000); 
    Serial.println("Cut motor moving to find home position");
  } else {
    Serial.println("ERROR: Cut motor stepper not initialized!");
    cutMotorHomed = true; 
  }

  Serial.println("Starting position motor homing...");
  if (positionMotorStepper) {
    positionMotorStepper->setSpeedInHz(POSITION_MOTOR_HOMING_SPEED);
    positionMotorStepper->setAcceleration(POSITION_MOTOR_ACCELERATION); 
    positionMotorStepper->move(-2000000000); 
    Serial.println("Position motor moving to find home position");
  } else {
    Serial.println("ERROR: Position motor stepper not initialized!");
    positionMotorHomed = true; 
  }

  Serial.println("Waiting for homing switches to activate...");
  unsigned long homingStartTime = millis();

  while (!cutMotorHomed || !positionMotorHomed) {
    cutMotorSwitch.update();
    positionMotorSwitch.update();

    if (!cutMotorHomed && cutMotorStepper && cutMotorSwitch.read() == HIGH) {
      Serial.println("Cut motor home switch ACTIVATED.");
      cutMotorStepper->stopMove();
      cutMotorStepper->setCurrentPosition(0);
      cutMotorHomed = true;
      Serial.println("Cut motor homed successfully.");
    }

    if (!positionMotorHomed && positionMotorStepper && positionMotorSwitch.read() == HIGH) {
      Serial.println("Position motor home switch ACTIVATED.");
      positionMotorStepper->stopMove();
      positionMotorStepper->setCurrentPosition(0); // Temporary zero at switch
      
      long oneInchSteps = (long)(1.0 * POSITION_MOTOR_STEPS_PER_INCH);
      Serial.print("Position motor moving 1 inch away from switch to relative: "); Serial.println(oneInchSteps);
      
      // Use a standard speed for this short move, ensure it's defined
      positionMotorStepper->setSpeedInHz(POSITION_MOTOR_NORMAL_SPEED); 
      positionMotorStepper->setAcceleration(POSITION_MOTOR_ACCELERATION);
      positionMotorStepper->moveTo(oneInchSteps); // Move 1 inch from temporary zero
      
      unsigned long motorMoveStartTime = millis();
      while (positionMotorStepper->isRunning()) {
        delay(0); // Yield for other tasks, though this is blocking for homing
      }
      
      positionMotorStepper->setCurrentPosition(0); // New zero is 1 inch away from the switch
      positionMotorHomed = true;
      Serial.println("Position motor homed and offset by 1 inch. New zero set.");

      positionMotorStepper->moveTo(3.45); // Move 3.45 inches from home switch
      while (positionMotorStepper->isRunning()) {
                delay(0); // Yield for other tasks, though this is blocking for homing
      }
      Serial.println("Position motor moved to 3.45 inches from home switch.");
    }
    
    delay(0); 
  }

  Serial.println("==== HOMING SEQUENCE COMPLETED ====");
  Serial.println("Transitioning to IDLE state");
  transitionToState(IDLE);
} 