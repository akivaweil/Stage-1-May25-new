#include "Cutting.h"
#include "settings.h"
#include <FastAccelStepper.h>
#include "StateMachine.h" // For state transitions

//* ************************************************************************
//* ***************************** CUTTING ********************************
//* ************************************************************************
// This file contains the definitions for the cutting state functions. 

void performCutCycle() {
  Serial.println("CUTTING: Engaging clamps...");
  digitalWrite(POSITION_CLAMP_PIN, HIGH);
  digitalWrite(SECURE_WOOD_CLAMP_PIN, HIGH);

  // Short delay to ensure clamps are engaged before movement
  // This is a blocking delay, consider if a non-blocking alternative is needed for complex state management
  delay(500); // 500 ms delay, adjust as necessary

  Serial.println("CUTTING: Moving cut motor for cutting operation...");
  if (cutMotorStepper) {
    long targetPositionSteps = (long)(CUT_MOTOR_TRAVEL_DISTANCE * CUT_MOTOR_STEPS_PER_INCH);
    
    cutMotorStepper->setSpeedInHz(CUT_MOTOR_CUTTING_SPEED);
    cutMotorStepper->setAcceleration(CUT_MOTOR_ACCELERATION);
    
    // Assuming the motor is at its home/start position (0) before cutting
    // And CUT_MOTOR_TRAVEL_DISTANCE is the distance to move *to* for the cut
    cutMotorStepper->moveTo(targetPositionSteps);
    
    unsigned long motorMoveStartTime = millis();
    while (cutMotorStepper->isRunning()) {
      // Yield or add a small delay if other tasks need to run, though this sequence is mostly blocking
      delay(0);
      if (millis() - motorMoveStartTime > 15000) { // 15-second timeout for cut travel
            Serial.println("ERROR: Timeout waiting for cut motor to complete travel!");
            cutMotorStepper->stopMove(); 
            break; 
        }
    }
    Serial.println("CUTTING: Cut motor movement complete.");
    
    // After the cut, the next step would typically be to return the cut motor.
    // This will be handled by subsequent instructions or states.

  } else {
    Serial.println("ERROR: Cut motor stepper not initialized!");
  }
  
  // After cutting operation, check for wood presence
  // Sensor is active LOW (LOW means wood, HIGH means no wood)
  // pinMode(YES_OR_NO_WOOD_SENSOR_PIN, INPUT_PULLDOWN); // Ensure pin is configured, could be done once in setup()
  delay(10); // Small delay for sensor reading to stabilize if needed
  int woodSensorState = digitalRead(YES_OR_NO_WOOD_SENSOR_PIN);

  Serial.print("CUTTING: Wood sensor state: ");
  Serial.println(woodSensorState == LOW ? "WOOD PRESENT (LOW)" : "NO WOOD (HIGH)");

  if (woodSensorState == LOW) { // Wood is present
    transitionToState(YES_WOOD);
  } else { // No wood is present
    transitionToState(NO_WOOD);
  }
  // Serial.println("CUTTING: Cut cycle part 1 (engage clamps and cut) finished."); // This message is now misleading
} 