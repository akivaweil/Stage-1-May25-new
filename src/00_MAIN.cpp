#include <Arduino.h>
#include "settings.h"
#include "StateMachine.h"
#include <FastAccelStepper.h>
#include "Homing.h"

//* ************************************************************************
//* ****************************** MAIN **********************************
//* ************************************************************************
// This file contains the main setup and loop functions for the automated table saw.

// Define FastAccelStepper engine and stepper objects
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper* cutMotorStepper = NULL;
FastAccelStepper* positionMotorStepper = NULL;

void setup() {
  Serial.begin(115200); // Initialize Serial for debugging
  delay(500); // Short delay to ensure serial port initializes
  
  Serial.println("=== System Startup ===");
  Serial.println("Initializing pins and stepper motors...");

  // Initialize Clamp Pins
  pinMode(POSITION_CLAMP_PIN, OUTPUT);
  pinMode(SECURE_WOOD_CLAMP_PIN, OUTPUT);
  digitalWrite(POSITION_CLAMP_PIN, LOW); // Start with clamps disengaged
  digitalWrite(SECURE_WOOD_CLAMP_PIN, LOW); // Start with clamps disengaged

  // Initialize Wood Sensor Pin
  pinMode(YES_OR_NO_WOOD_SENSOR_PIN, INPUT_PULLDOWN);

  Serial.println("Initializing FastAccelStepper engine...");
  engine.init();

  Serial.println("Connecting Cut Motor Stepper...");
  cutMotorStepper = engine.stepperConnectToPin(CUT_MOTOR_PULSE_PIN);
  if (cutMotorStepper) {
    cutMotorStepper->setDirectionPin(CUT_MOTOR_DIR_PIN);
    Serial.println("Cut Motor Stepper connected successfully");
    // cutMotorStepper->setEnablePin(CUT_MOTOR_ENABLE_PIN); // Enable pin not used as per settings
    // cutMotorStepper->setAutoEnable(true); // Decide if you want auto-enable
    // cutMotorStepper->setDirectionPinHighIsForward(true); // Set based on your wiring
  } else {
    Serial.println("ERROR: Failed to connect Cut Motor Stepper!");
  }

  Serial.println("Connecting Position Motor Stepper...");
  positionMotorStepper = engine.stepperConnectToPin(POSITION_MOTOR_PULSE_PIN);
  if (positionMotorStepper) {
    positionMotorStepper->setDirectionPin(POSITION_MOTOR_DIR_PIN);
    Serial.println("Position Motor Stepper connected successfully");
    // positionMotorStepper->setEnablePin(POSITION_MOTOR_ENABLE_PIN); // Enable pin not used
    // positionMotorStepper->setAutoEnable(true);
    // positionMotorStepper->setDirectionPinHighIsForward(true); // Set based on your wiring
  } else {
    Serial.println("ERROR: Failed to connect Position Motor Stepper!");
  }
  
  Serial.println("Initializing State Machine...");
  initializeStateMachine(); // Initialize the state machine

  Serial.println("Homing sequence starting...");
  runHomingSequence(); // This will now transition to IDLE state internally
  Serial.println("Homing sequence completed and system should be in IDLE state.");

  // Other setup code here
}

void loop() {
  runStateMachine();
} 