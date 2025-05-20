#include "Idle.h"
#include "settings.h"
#include "StateMachine.h" // For potential future transitions out of IDLE
#include <Arduino.h> // For Serial
#include <Bounce2.h> // Include Bounce2 library

//* ************************************************************************
//* ******************************* IDLE *********************************
//* ************************************************************************
// This file contains the definitions for the IDLE state functions.

Bounce cycleSwitch = Bounce(); // Create a Bounce object for the cycle switch

void enterIdleState() {
  Serial.println("ENTERING IDLE STATE");
  // Perform any actions needed when entering IDLE state
  // e.g., turn off motors, set status LEDs

  // Setup Cycle Switch
  cycleSwitch.attach(CYCLE_SWITCH_PIN, INPUT_PULLDOWN);
  cycleSwitch.interval(25); // Debounce interval of 25ms
  Serial.println("IDLE: Cycle switch initialized.");
}

void runIdleState() {
  cycleSwitch.update(); // Update the Bounce object

  if (cycleSwitch.read() == HIGH) { // If cycle switch is pressed (HIGH)
    Serial.println("IDLE: Cycle switch activated. Transitioning to CUTTING.");
    transitionToState(CUTTING);
  }
  // Other idle tasks can go here, but avoid blocking delays
} 