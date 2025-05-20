#pragma once
#include <Arduino.h>

//* ************************************************************************
//* ****************************** HOMING ********************************
//* ************************************************************************
// This file contains the declarations for the homing state functions. 

void runHomingSequence();
void homeCutMotor();
void homePositionMotor();