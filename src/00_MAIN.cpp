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

// --- LED Control Function Stubs ---
void setYellowLed(bool state) {
  Serial.print("STUB: setYellowLed called with state: "); Serial.println(state);
  // digitalWrite(YELLOW_LED_PIN, state ? HIGH : LOW); // Example implementation
}
void setGreenLed(bool state) {
  Serial.print("STUB: setGreenLed called with state: "); Serial.println(state);
  // digitalWrite(GREEN_LED_PIN, state ? HIGH : LOW);
}
void setBlueLed(bool state) {
  Serial.print("STUB: setBlueLed called with state: "); Serial.println(state);
  // digitalWrite(BLUE_LED_PIN, state ? HIGH : LOW);
}
void setRedLed(bool state) {
  Serial.print("STUB: setRedLed called with state: "); Serial.println(state);
  // digitalWrite(RED_LED_PIN, state ? HIGH : LOW);
}

// --- Motor Configuration Function Stubs ---
void configureCutMotorForReturn() {
  Serial.println("STUB: configureCutMotorForReturn called");
  // if (cutMotorStepper) { cutMotorStepper->setSpeedInHz(CUT_MOTOR_RETURN_SPEED); cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION); }
}
void configurePositionMotorForReturn() {
  Serial.println("STUB: configurePositionMotorForReturn called");
  // if (positionMotorStepper) { positionMotorStepper->setSpeedInHz(POSITION_MOTOR_RETURN_SPEED); positionMotorStepper->setAcceleration(POSITION_MOTOR_ACCELERATION); }
}
void configureCutMotorForNormalOperation() {
  Serial.println("STUB: configureCutMotorForNormalOperation called");
  // if (cutMotorStepper) { cutMotorStepper->setSpeedInHz(CUT_MOTOR_NORMAL_SPEED); cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION); }
}
void configurePositionMotorForNormalOperation() {
  Serial.println("STUB: configurePositionMotorForNormalOperation called");
  // if (positionMotorStepper) { positionMotorStepper->setSpeedInHz(POSITION_MOTOR_NORMAL_SPEED); positionMotorStepper->setAcceleration(POSITION_MOTOR_ACCELERATION); }
}

// --- Motor Movement Function Stubs ---
void moveCutMotorToPositionInches(float positionInches) {
  Serial.print("STUB: moveCutMotorToPositionInches called with inches: "); Serial.println(positionInches);
  // if (cutMotorStepper) { cutMotorStepper->moveTo(positionInches * CUT_MOTOR_STEPS_PER_INCH); }
}
void movePositionMotorToPositionInches(float positionInches) {
  Serial.print("STUB: movePositionMotorToPositionInches called with inches: "); Serial.println(positionInches);
  // if (positionMotorStepper) { positionMotorStepper->moveTo(positionInches * POSITION_MOTOR_STEPS_PER_INCH); }
}

// --- Motor Status Function Stubs ---
bool isCutMotorAtTarget() { 
  Serial.println("STUB: isCutMotorAtTarget called, returning true");
  // return cutMotorStepper ? !cutMotorStepper->isRunning() : true; // Simplified stub
  return true; // Placeholder
}
bool isPositionMotorAtTarget() { 
  Serial.println("STUB: isPositionMotorAtTarget called, returning true");
  // return positionMotorStepper ? !positionMotorStepper->isRunning() : true; // Simplified stub
  return true; // Placeholder
}

// --- Switch and Sensor Function Stubs ---
bool isCutMotorAtHome() {
  Serial.println("STUB: isCutMotorAtHome called, returning true");
  // return digitalRead(CUT_MOTOR_HOMING_SWITCH_PIN) == LOW; // Example, assuming active LOW
  return true; // Placeholder
}
bool readPositionMotorHomingSwitch() {
  Serial.println("STUB: readPositionMotorHomingSwitch called, returning true");
  // return digitalRead(POSITION_MOTOR_HOMING_SWITCH_PIN) == LOW; // Example, assuming active LOW
  return true; // Placeholder
}

// --- Clamp Control Function Definitions ---
void extendSecureWoodClamp() {
    digitalWrite(SECURE_WOOD_CLAMP_PIN, HIGH);
    Serial.println("Secure wood clamp extended.");
}

void retractSecureWoodClamp() {
    digitalWrite(SECURE_WOOD_CLAMP_PIN, LOW);
    Serial.println("Secure wood clamp retracted.");
}

void extendPositionClamp() {
    digitalWrite(POSITION_CLAMP_PIN, HIGH);
    Serial.println("Position clamp extended.");
}

void retractPositionClamp() {
    digitalWrite(POSITION_CLAMP_PIN, LOW);
    Serial.println("Position clamp retracted.");
} 