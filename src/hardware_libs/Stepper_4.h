
//      ******************************************************************
//      *                                                                *
//      *                    Header file for Stepper_4                   *
//      *            Juan Tinazzo 2022, based on the library             *
//      *                       ESP - FlexyStepper by                    *
//      *            Paul Kerspe                     4.6.2020            *
//      *       based on the concept of FlexyStepper by Stan Reifel      *
//      *                                                                *
//      ******************************************************************

// MIT License
//
// Copyright (c) 2022 Juan Tinazzo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This library is based on the works of Paul Kerspe in his ESP-FlexyStepper library:
// https://github.com/pkerspe/ESP-FlexyStepper

#ifndef STEPPER_4_h
#define STEPPER_4_h

#ifdef ESP32
//
//#elif defined(ESP8266)
//
#else
#error Platform not supported, only ESP32 modules are currently supported
#endif

#include <Arduino.h>
#include <stdlib.h>

typedef void (*callbackFunction)(void);
typedef void (*positionCallbackFunction)(long);

class Stepper_4
{
public:
    Stepper_4();
    ~Stepper_4();
    // service functions
    bool startAsService(int coreNumber = 1);
    void stopService(void);
    bool isStartedAsService(void);

    long getTaskStackHighWaterMark();
    // IO setup and helper / debugging functions
    void setSS(byte SS_p);
    bool motionComplete();
    int getDirectionOfMotion(void);
    bool isMovingTowardsHome(void);
    bool processMovement(void);

    // register function for callbacks
    void registerHomeReachedCallback(callbackFunction homeReachedCallbackFunction);
    void registerTargetPositionReachedCallback(positionCallbackFunction targetPositionReachedCallbackFunction);

    // configuration functions
    void setStepsPerMillimeter(float motorStepPerMillimeter);
    void setStepsPerRevolution(float motorStepPerRevolution);
    void setSpeedInStepsPerSecond(float speedInStepsPerSecond);
    void setSpeedInMillimetersPerSecond(float speedInMillimetersPerSecond);
    void setSpeedInRevolutionsPerSecond(float speedInRevolutionsPerSecond);
    void setAccelerationInMillimetersPerSecondPerSecond(float accelerationInMillimetersPerSecondPerSecond);
    void setAccelerationInRevolutionsPerSecondPerSecond(float accelerationInRevolutionsPerSecondPerSecond);
    void setDecelerationInMillimetersPerSecondPerSecond(float decelerationInMillimetersPerSecondPerSecond);
    void setDecelerationInRevolutionsPerSecondPerSecond(float decelerationInRevolutionsPerSecondPerSecond);
    void setAccelerationInStepsPerSecondPerSecond(float accelerationInStepsPerSecondPerSecond);
    void setDecelerationInStepsPerSecondPerSecond(float decelerationInStepsPerSecondPerSecond);
    void setDirectionToHome(signed char directionTowardHome);

    float getCurrentVelocityInStepsPerSecond();
    float getCurrentVelocityInRevolutionsPerSecond();
    float getCurrentVelocityInMillimetersPerSecond(void);

    // positioning functions
    void setCurrentPositionInSteps(long currentPositionInSteps);
    void setCurrentPositionInMillimeters(float currentPositionInMillimeters);
    void setCurrentPositionInRevolutions(float currentPositionInRevolutions);

    long getCurrentPositionInSteps();
    float getCurrentPositionInRevolutions();
    float getCurrentPositionInMillimeters();

    void startJogging(signed char direction);
    void stopJogging();
    void goToLimitAndSetAsHome(callbackFunction callbackFunctionForHome = NULL, long maxDistanceToMoveInSteps = 2000000000L);
    void goToLimit(signed char direction, callbackFunction callbackFunctionForLimit = NULL);

    void setCurrentPositionAsHomeAndStop(void);
    void setTargetPositionToStop();
    long getDistanceToTargetSigned(void);

    void setTargetPositionInSteps(long absolutePositionToMoveToInSteps);
    void setTargetPositionInMillimeters(float absolutePositionToMoveToInMillimeters);
    void setTargetPositionInRevolutions(float absolutePositionToMoveToInRevolutions);
    void setTargetPositionRelativeInSteps(long distanceToMoveInSteps);
    void setTargetPositionRelativeInMillimeters(float distanceToMoveInMillimeters);
    void setTargetPositionRelativeInRevolutions(float distanceToMoveInRevolutions);

    long getTargetPositionInSteps();
    float getTargetPositionInMillimeters();
    float getTargetPositionInRevolutions();

    // blocking function calls
    void moveToPositionInSteps(long absolutePositionToMoveToInSteps);
    void moveToPositionInMillimeters(float absolutePositionToMoveToInMillimeters);
    void moveToPositionInRevolutions(float absolutePositionToMoveToInRevolutions);
    void moveRelativeInSteps(long distanceToMoveInSteps);
    void moveRelativeInMillimeters(float distanceToMoveInMillimeters);
    void moveRelativeInRevolutions(float distanceToMoveInRevolutions);

    bool moveToHomeInSteps(signed char directionTowardHome, float speedInStepsPerSecond, long maxDistanceToMoveInSteps, int homeSwitchPin);
    bool moveToHomeInMillimeters(signed char directionTowardHome, float speedInMillimetersPerSecond, long maxDistanceToMoveInMillimeters, int homeLimitSwitchPin);
    bool moveToHomeInRevolutions(signed char directionTowardHome, float speedInRevolutionsPerSecond, long maxDistanceToMoveInRevolutions, int homeLimitSwitchPin);

    static const byte ACTIVE_HIGH = 1;
    static const byte ACTIVE_LOW = 2;

private:
    callbackFunction _homeReachedCallback = NULL;
    positionCallbackFunction _targetPositionReachedCallback = NULL;
    callbackFunction _callbackFunctionForGoToLimit = NULL;

    static void taskRunner(void *parameter);

    void DeterminePeriodOfNextStep();
    void triggerBrakeIfNeededOrSetTimeout(void);

    byte SS;
    // byte directionPin;
    float stepsPerMillimeter;
    float stepsPerRevolution;
    int directionOfMotion;
    long currentPosition_InSteps;
    long targetPosition_InSteps;
    float desiredSpeed_InStepsPerSecond;
    float desiredPeriod_InUSPerStep;
    float acceleration_InStepsPerSecondPerSecond;
    float acceleration_InStepsPerUSPerUS;
    float deceleration_InStepsPerSecondPerSecond;
    float deceleration_InStepsPerUSPerUS;
    float periodOfSlowestStep_InUS;
    float minimumPeriodForAStoppedMotion;
    float nextStepPeriod_InUS;
    unsigned long lastStepTime_InUS;
    float currentStepPeriod_InUS;
    signed char directionTowardsHome;
    signed char lastStepDirectionBeforeLimitSwitchTrigger;
    // true if the current stepper positon equals the homing position
    bool isCurrentlyHomed;
    bool isOnWayToHome = false;
    bool isOnWayToLimit = false;
    bool firstProcessingAfterTargetReached = true;
    // The type ID of the limit switch type that is active. possible values are LIMIT_SWITCH_BEGIN (-1) or LIMIT_SWITCH_END (1) or LIMIT_SWITCH_COMBINED_BEGIN_AND_END (2) or 0 if no limit switch is active
    signed char activeLimitSwitch;
    bool limitSwitchCheckPeformed;
    // 0 if the the stepper is allowed to move in both directions (e.g. no limit or homing switch triggered), otherwise indicated which direction is currently not allowed for further movement
    signed char disallowedDirection;

    TaskHandle_t xHandle = NULL;

    byte stepSeq[4] = {0b0011, 0b0110, 0b1100, 0b1001};
    byte stepPos = 0;
};

// ------------------------------------ End ---------------------------------
#endif
