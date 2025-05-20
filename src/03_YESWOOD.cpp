#include "YesWood.h"
#include "settings.h" 
#include <Arduino.h> 

//* ************************************************************************
//* ************************** YES WOOD STATE ******************************
//* ************************************************************************
// This file contains the logic for the "Yes Wood" operational state.
// It handles the sequence of actions when wood is detected and the cycle is initiated.

// Assumed clamp control functions (defined in 00_MAIN.cpp)
extern void extendSecureWoodClamp();
extern void retractSecureWoodClamp();
extern void extendPositionClamp();
extern void retractPositionClamp();

void showYesWoodIndicator() {
    // TODO: Implement what showing the YesWood indicator means, e.g., turn on a specific LED.
    Serial.println("YesWood State: Indicator activated.");
    // Example: setGreenLed(true); 
}

void handleYesWoodState() {
    //! 1. Retract the secure wood clamp
    Serial.println("YesWood State: Retracting secure wood clamp.");
    retractSecureWoodClamp();

    //! 2. Move the position motor to POSITION_MOTOR_TRAVEL_DISTANCE - 0.1
    float targetPositionStep2 = POSITION_MOTOR_TRAVEL_DISTANCE - 0.1;
    Serial.print("YesWood State: Moving position motor to ");
    Serial.print(targetPositionStep2);
    Serial.println(" inches.");
    configurePositionMotorForNormalOperation(); 
    movePositionMotorToPositionInches(targetPositionStep2);
    while (!isPositionMotorAtTarget()) {
        delay(10); 
    }
    Serial.println("YesWood State: Position motor reached target for step 2.");

    //! 3. Retract the position clamp and extend the secure wood clamp
    Serial.println("YesWood State: Retracting position clamp and extending secure wood clamp.");
    retractPositionClamp();
    extendSecureWoodClamp();

    //! 4. Both motors should now return to the zero position together
    Serial.println("YesWood State: Returning both motors to home.");
    configureCutMotorForReturn();
    configurePositionMotorForReturn();
    moveCutMotorToPositionInches(0);
    movePositionMotorToPositionInches(0);

    bool cutMotorHome = false;
    bool positionMotorHome = false;

    // Non-blocking wait for both motors to reach home
    // Also handles extending position clamp and retracting secure wood clamp at the right times
    unsigned long motorCheckStartTime = millis();
    while (!cutMotorHome || !positionMotorHome) {
        if (!positionMotorHome && isPositionMotorAtTarget()) {
            if (positionMotorStepper->getCurrentPosition() == 0) { // Double check it's actually at zero
                //! 5. As soon as the position motor reaches position zero the position clamp should extend.
                Serial.println("YesWood State: Position motor reached home. Extending position clamp.");
                extendPositionClamp();
                positionMotorHome = true;
            }
        }
        if (!cutMotorHome && isCutMotorAtTarget()) {
             if (cutMotorStepper->getCurrentPosition() == 0) { // Double check it's actually at zero
                //! 6. When the cut motor reaches home, the secure wood clamp should retract
                Serial.println("YesWood State: Cut motor reached home. Retracting secure wood clamp.");
                retractSecureWoodClamp();
                cutMotorHome = true;
            }
        }
        
        // Optional: Add a timeout for motor homing if necessary, to prevent infinite loop
        // if (millis() - motorCheckStartTime > MAX_HOMING_TIMEOUT) { 
        //     Serial.println("Error: Timeout waiting for motors to home in YesWoodState.");
        //     // Handle error, e.g., transition to an error state
        //     break; 
        // }
        delay(10); // Small delay to yield to other processes and check motor status
    }
    Serial.println("YesWood State: Both motors reached home.");

    //! 7. The position motor moves to position POSITION_MOTOR_TRAVEL_DISTANCE
    Serial.print("YesWood State: Moving position motor to ");
    Serial.print(POSITION_MOTOR_TRAVEL_DISTANCE);
    Serial.println(" inches.");
    configurePositionMotorForNormalOperation();
    movePositionMotorToPositionInches(POSITION_MOTOR_TRAVEL_DISTANCE);
    while (!isPositionMotorAtTarget()) {
        delay(10); 
    }
    Serial.println("YesWood State: Position motor reached final target. State complete.");
    
    // After completing the YesWood sequence, you might want to transition to another state, e.g., IDLE
    // transitionToState(IDLE); // This would require transitionToState to be accessible here
}
