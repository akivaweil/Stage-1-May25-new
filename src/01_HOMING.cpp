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

  cutMotorSwitch.interval(10); // 10 ms debounce interval
  positionMotorSwitch.interval(10); // 10 ms debounce interval

  // Force an initial update of switch states
  cutMotorSwitch.update();
  positionMotorSwitch.update();
  
  Serial.print("Cut Motor Switch initial state: ");
  Serial.println(cutMotorSwitch.read() ? "HIGH" : "LOW");
  Serial.print("Position Motor Switch initial state: ");
  Serial.println(positionMotorSwitch.read() ? "HIGH" : "LOW");

  bool cutMotorHomed = false;
  bool positionMotorHomed = false;
  
  // Keep track of how far the cut motor has traveled
  long maxCutTravel = (long)(10.0 * CUT_MOTOR_STEPS_PER_INCH); // Maximum distance to travel (10 inches)
  unsigned long cutHomingStartTime = millis();

  Serial.println("Starting cut motor homing...");
  if (cutMotorStepper) {
    cutMotorStepper->setSpeedInHz(CUT_MOTOR_HOMING_SPEED);
    cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION); 
    cutMotorStepper->move(-maxCutTravel); // Move a maximum distance instead of "infinity"
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
  unsigned long lastDebugOutputTime = 0;
  const unsigned long DEBUG_INTERVAL = 5000; // Print debug every 5 seconds
  const unsigned long CUT_MOTOR_HOME_TIMEOUT = 30000; // 30 seconds max for cut motor

  while (!cutMotorHomed || !positionMotorHomed) {
    cutMotorSwitch.update();
    positionMotorSwitch.update();

    // Debug output every few seconds
    unsigned long currentTime = millis();
    if (currentTime - lastDebugOutputTime > DEBUG_INTERVAL) {
      Serial.print("Cut switch state: ");
      Serial.print(cutMotorSwitch.read() ? "HIGH" : "LOW");
      Serial.print(" | Position switch state: ");
      Serial.println(positionMotorSwitch.read() ? "HIGH" : "LOW");
      lastDebugOutputTime = currentTime;
    }

    // If cut motor switch activates
    if (!cutMotorHomed && cutMotorStepper && cutMotorSwitch.read() == HIGH) {
      Serial.println("Cut motor home switch ACTIVATED.");
      cutMotorStepper->stopMove();
      cutMotorStepper->setCurrentPosition(0);
      cutMotorHomed = true;
      Serial.println("Cut motor homed successfully.");
    }
    
    // If cut motor times out or has reached end of travel
    if (!cutMotorHomed && cutMotorStepper && 
        ((millis() - cutHomingStartTime > CUT_MOTOR_HOME_TIMEOUT) || !cutMotorStepper->isRunning())) {
      Serial.println("Cut motor homing timed out or reached end of travel distance.");
      Serial.println("Assuming cut motor is at home position.");
      cutMotorStepper->stopMove();
      cutMotorStepper->setCurrentPosition(0);
      cutMotorHomed = true;
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
        
        // Continue checking cut motor switch during this time
        if (!cutMotorHomed) {
          cutMotorSwitch.update();
          if (cutMotorSwitch.read() == HIGH) {
            Serial.println("Cut motor home switch ACTIVATED during position motor movement.");
            cutMotorStepper->stopMove();
            cutMotorStepper->setCurrentPosition(0);
            cutMotorHomed = true;
            Serial.println("Cut motor homed successfully.");
          }
          
          // Also check for timeout during this waiting period
          if ((millis() - cutHomingStartTime > CUT_MOTOR_HOME_TIMEOUT) || !cutMotorStepper->isRunning()) {
            Serial.println("Cut motor homing timed out or reached end of travel during position motor movement.");
            Serial.println("Assuming cut motor is at home position.");
            cutMotorStepper->stopMove();
            cutMotorStepper->setCurrentPosition(0);
            cutMotorHomed = true;
          }
        }
        
        if (millis() - motorMoveStartTime > 10000) { // Timeout for the 1-inch move (10 seconds)
            Serial.println("ERROR: Timeout waiting for position motor to move 1 inch from home switch!");
            positionMotorStepper->stopMove(); // Stop the motor
            break; // Exit the while(isRunning) loop
        }
      }
      
      positionMotorStepper->setCurrentPosition(0); // New zero is 1 inch away from the switch
      positionMotorHomed = true;
      Serial.println("Position motor homed and offset by 1 inch. New zero set.");
    }
    
    // Add a check for excessive time spent homing (safety feature)
    if (millis() - homingStartTime > 60000) { // 60 second timeout
        Serial.println("ERROR: Homing sequence timed out after 60 seconds!");
        
        // Give specific information about which motors failed to home
        if (!cutMotorHomed) {
            Serial.println("Cut motor failed to find home position!");
            cutMotorStepper->stopMove();
            cutMotorStepper->setCurrentPosition(0);
            cutMotorHomed = true;
        }
        if (!positionMotorHomed) {
            Serial.println("Position motor failed to find home position!");
            positionMotorStepper->stopMove();
            positionMotorStepper->setCurrentPosition(0);
            positionMotorHomed = true;
        }
        break;
    }
    
    delay(0); 
  }

  // Final offset for the cut motor if needed - similar to what we do for position motor
  if (cutMotorStepper && cutMotorHomed) {
    // You might want to add a small offset for the cut motor as well
    long cutMotorOffset = (long)(0.5 * CUT_MOTOR_STEPS_PER_INCH); // 0.5 inch offset example
    Serial.print("Moving cut motor to offset position: ");
    Serial.println(cutMotorOffset);
    
    cutMotorStepper->setSpeedInHz(CUT_MOTOR_NORMAL_SPEED);
    cutMotorStepper->moveTo(cutMotorOffset);
    
    while (cutMotorStepper->isRunning()) {
      delay(0);
    }
    
    cutMotorStepper->setCurrentPosition(0);
    Serial.println("Cut motor at offset position, new zero set.");
  }

  Serial.println("==== HOMING SEQUENCE COMPLETED ====");
  Serial.println("Transitioning to IDLE state");
  transitionToState(IDLE);
} 