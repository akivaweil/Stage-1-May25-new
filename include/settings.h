#pragma once
#include <Arduino.h>
#include <FastAccelStepper.h>

//* ************************************************************************
//* ***************************** SETTINGS *******************************
//* ************************************************************************
// This file contains global settings, constants, and pin definitions for the project. 

// Declare global FastAccelStepper pointers (to be defined in main.cpp)
extern FastAccelStepper* cutMotorStepper;
extern FastAccelStepper* positionMotorStepper;

// Motor Constants
const float CUT_MOTOR_STEPS_PER_INCH = 500;
const float POSITION_MOTOR_STEPS_PER_INCH = 1000;

// Travel Distances
const float CUT_MOTOR_TRAVEL_DISTANCE = 5.00;  // inches
const float POSITION_MOTOR_TRAVEL_DISTANCE = 3.45;  // inches

// Accelerations
const float CUT_MOTOR_ACCELERATION = 20000;  // steps/sec²
const float POSITION_MOTOR_ACCELERATION = 30000;  // steps/sec²

// --- Speeds by State ---

// Homing State
const float CUT_MOTOR_HOMING_SPEED = 166;  // steps/sec
const float POSITION_MOTOR_HOMING_SPEED = 166;  // steps/sec

// Cutting State
const float CUT_MOTOR_CUTTING_SPEED = 1000;  // steps/sec - slower speed for precise cutting

// Normal Operation / Return Speeds (can be categorized further if needed by other states)
const float CUT_MOTOR_NORMAL_SPEED = 2000;  // steps/sec
const float CUT_MOTOR_RETURN_SPEED = 2000;  // steps/sec
const float POSITION_MOTOR_NORMAL_SPEED = 2000;  // steps/sec
const float POSITION_MOTOR_RETURN_SPEED = 2000;  // steps/sec

// Operational Constants
const float WAS_WOOD_SUCTIONED_POSITION = 0.3;  // inches
const float TRANSFER_ARM_SIGNAL_POSITION = 7.2;  // inches 

// Motor Pin Definitions
#define CUT_MOTOR_PULSE_PIN 12
#define CUT_MOTOR_DIR_PIN 11

#define POSITION_MOTOR_PULSE_PIN 17
#define POSITION_MOTOR_DIR_PIN 18


// Switch and Sensor Pin Definitions
#define CUT_MOTOR_HOMING_SWITCH_PIN 3
#define POSITION_MOTOR_HOMING_SWITCH_PIN 16
#define RELOAD_SWITCH_PIN 6
#define CYCLE_SWITCH_PIN 5
#define YES_OR_NO_WOOD_SENSOR_PIN 10
#define WAS_WOOD_SUCTIONED_SENSOR_PIN 37
// CUT_MOTOR_EMERGENCY_SWITCH_PIN was removed as per user request

// Clamp Pin Definitions
#define POSITION_CLAMP_PIN 2
#define SECURE_WOOD_CLAMP_PIN 35

// Signal Pin Definition
#define SIGNAL_TO_TRANSFER_ARM_PIN 36

// LED Pin Definitions
#define RED_LED_PIN 45
#define YELLOW_LED_PIN 48
#define GREEN_LED_PIN 47
#define BLUE_LED_PIN 21 