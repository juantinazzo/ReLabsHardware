#include "Stepper_4.h"
#include <Arduino.h>
#include <SPI.h>

#define POSITIVE_DIRECTION LOW
#define NEGATIVE_DIRECTION HIGH

SPIClass SPI1(HSPI);

Stepper_4::Stepper_4()
{
    this->lastStepTime_InUS = 0L;
    this->stepsPerRevolution = 200L;
    this->stepsPerMillimeter = 25.0;
    this->directionOfMotion = 0;
    this->currentPosition_InSteps = 0L;
    this->targetPosition_InSteps = 0L;
    this->setSpeedInStepsPerSecond(200);
    this->setAccelerationInStepsPerSecondPerSecond(200.0);
    this->setDecelerationInStepsPerSecondPerSecond(200.0);
    this->currentStepPeriod_InUS = 0.0;
    this->nextStepPeriod_InUS = 0.0;
    this->isCurrentlyHomed = false;
    this->directionTowardsHome = -1;
    this->disallowedDirection = 0;
    this->activeLimitSwitch = 0; // see LIMIT_SWITCH_BEGIN and LIMIT_SWITCH_END
    this->lastStepDirectionBeforeLimitSwitchTrigger = 0;
    this->limitSwitchCheckPeformed = false;

    SPI1.begin();
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
}

Stepper_4::~Stepper_4()
{
    if (this->xHandle != NULL)
    {
        this->stopService();
    }
}

// TODO: use https://github.com/nrwiersma/ESP8266Scheduler/blob/master/examples/simple/simple.ino for ESP8266
bool Stepper_4::startAsService(int coreNumber)
{
    if (coreNumber == 1)
    {
        disableCore1WDT(); // we have to disable the Watchdog timer to prevent it from rebooting the ESP all the time another option would be to add a vTaskDelay but it would slow down the stepper
    }
    else if (coreNumber == 0)
    {
        disableCore0WDT(); // we have to disable the Watchdog timer to prevent it from rebooting the ESP all the time another option would be to add a vTaskDelay but it would slow down the stepper
    }
    else
    {
        // invalid core number given
        return false;
    }

    xTaskCreatePinnedToCore(
        Stepper_4::taskRunner, /* Task function. */
        "Stepper_4",           /* String with name of task (by default max 16 characters long) */
        2000,                  /* Stack size in bytes. */
        this,                  /* Parameter passed as input of the task */
        1,                     /* Priority of the task, 1 seems to work just fine for us */
        &this->xHandle,        /* Task handle. */
        coreNumber             /* the cpu core to use, 1 is where usually the Arduino Framework code (setup and loop function) are running, core 0 by default runs the Wifi Stack */
    );
    return true;
}

void Stepper_4::taskRunner(void *parameter)
{
    Stepper_4 *stepperRef = static_cast<Stepper_4 *>(parameter);
    for (;;)
    {
        stepperRef->processMovement();
        // vTaskDelay(1); // This would be a working solution to prevent the WDT to fire (if not disabled, yet it will cause noticeably less smooth stepper movements / lower frequencies)
    }
}

void Stepper_4::stopService(void)
{
    vTaskDelete(this->xHandle);
    this->xHandle = NULL;
}

bool Stepper_4::isStartedAsService()
{
    return (this->xHandle != NULL);
}

/**
 * get the overall max stack size since task creation (since the call to startAsService() )
 * This function is used to determine if the stacksize is large enough and has more of a debugging purpose.
 * Return the minimum amount of free bytes on the stack that has been measured so far.
 */
long Stepper_4::getTaskStackHighWaterMark()
{
    if (this->isStartedAsService())
    {
        return uxTaskGetStackHighWaterMark(this->xHandle);
    }
    return 0;
}

/**
 * get the distance in steps to the currently set target position.
 * 0 is returned if the stepper is already at the target position.
 * The returned value is signed, depending on the direction to move to reach the target
 */
long Stepper_4::getDistanceToTargetSigned()
{
    return (this->targetPosition_InSteps - this->currentPosition_InSteps);
}

/**
 *  configure the direction in which to move to reach the home position
 *  Accepts 1 or -1 as allowed values. Other values will be ignored
 */
void Stepper_4::setDirectionToHome(signed char directionTowardHome)
{
    if (directionTowardHome == -1 || directionTowardHome == 1)
    {
        this->directionTowardsHome = directionTowardHome;
    }
}

/**
 * get the current direction of motion of the connected stepper motor
 * returns 1 for "forward" motion
 * returns -1 for "backward" motion
 * returns 0 if the stepper has reached its destination position and is not moving anymore
 */
int Stepper_4::getDirectionOfMotion(void)
{
    return this->directionOfMotion;
}

/**
 * returns true if the stepper is currently in motion and moving in the direction of the home position.
 * Depends on the settings of setDirectionToHome() which defines where "home" is...a rather philosophical question :-)
 */
bool Stepper_4::isMovingTowardsHome()
{
    return (this->directionOfMotion == this->directionTowardsHome);
}

/*
 * connect the stepper object to the IO pins
 * stepPinNumber = IO pin number for the Step signale
 * directionPinNumber = IO pin number for the direction signal
 */
void Stepper_4::setSS(byte SS_p)
{
    this->SS = SS_p;
    // configure the IO pins
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
}

// ---------------------------------------------------------------------------------
//                     Public functions with units in millimeters
// ---------------------------------------------------------------------------------

//
// set the number of steps the motor has per millimeters
//
void Stepper_4::setStepsPerMillimeter(float motorStepsPerMillimeter)
{
    stepsPerMillimeter = motorStepsPerMillimeter;
}

//
// get the current position of the motor in millimeters, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in millimeters returned
//
float Stepper_4::getCurrentPositionInMillimeters()
{
    return ((float)getCurrentPositionInSteps() / stepsPerMillimeter);
}

//
// set the current position of the motor in millimeters.
// Do not confuse this function with setTargetPositionInMillimeters(), it does not directly cause a motor movement per se.
// NOTE: if you called one of the move functions before (and by that setting a target position internally) you might experience that the motor starts to move after calling setCurrentPositionInMillimeters() in the case that the value of currentPositionInMillimeters is different from the target position of the stepper. If this is not intended, you should call setTargetPositionInMillimeters() with the same value as the setCurrentPositionInMillimeters() function directly before or after calling setCurrentPositionInMillimeters
//
void Stepper_4::setCurrentPositionInMillimeters(
    float currentPositionInMillimeters)
{
    setCurrentPositionInSteps((long)round(currentPositionInMillimeters *
                                          stepsPerMillimeter));
}

//
// set the maximum speed, units in millimeters/second, this is the maximum speed
// reached while accelerating
//  Enter:  speedInMillimetersPerSecond = speed to accelerate up to, units in
//            millimeters/second
//
void Stepper_4::setSpeedInMillimetersPerSecond(float speedInMillimetersPerSecond)
{
    setSpeedInStepsPerSecond(speedInMillimetersPerSecond * stepsPerMillimeter);
}

//
// set the rate of acceleration, units in millimeters/second/second
//  Enter:  accelerationInMillimetersPerSecondPerSecond = rate of acceleration,
//          units in millimeters/second/second
//
void Stepper_4::setAccelerationInMillimetersPerSecondPerSecond(
    float accelerationInMillimetersPerSecondPerSecond)
{
    setAccelerationInStepsPerSecondPerSecond(
        accelerationInMillimetersPerSecondPerSecond * stepsPerMillimeter);
}

//
// set the rate of deceleration, units in millimeters/second/second
//  Enter:  decelerationInMillimetersPerSecondPerSecond = rate of deceleration,
//          units in millimeters/second/second
//
void Stepper_4::setDecelerationInMillimetersPerSecondPerSecond(
    float decelerationInMillimetersPerSecondPerSecond)
{
    setDecelerationInStepsPerSecondPerSecond(
        decelerationInMillimetersPerSecondPerSecond * stepsPerMillimeter);
}

//
// home the motor by moving until the homing sensor is activated, then set the
// position to zero, with units in millimeters
//  Enter:  directionTowardHome = 1 to move in a positive direction, -1 to move
//            in a negative directions
//          speedInMillimetersPerSecond = speed to accelerate up to while moving
//            toward home, units in millimeters/second
//          maxDistanceToMoveInMillimeters = unsigned maximum distance to move
//            toward home before giving up
//          homeSwitchPin = pin number of the home switch, switch should be
//            configured to go low when at home
//  Exit:   true returned if successful, else false
//
bool Stepper_4::moveToHomeInMillimeters(signed char directionTowardHome,
                                        float speedInMillimetersPerSecond, long maxDistanceToMoveInMillimeters,
                                        int homeLimitSwitchPin)
{
    return (moveToHomeInSteps(directionTowardHome,
                              speedInMillimetersPerSecond * stepsPerMillimeter,
                              maxDistanceToMoveInMillimeters * stepsPerMillimeter,
                              homeLimitSwitchPin));
}

//
// move relative to the current position, units are in millimeters, this function
// does not return until the move is complete
//  Enter:  distanceToMoveInMillimeters = signed distance to move relative to the
//          current position in millimeters
//
void Stepper_4::moveRelativeInMillimeters(float distanceToMoveInMillimeters)
{
    setTargetPositionRelativeInMillimeters(distanceToMoveInMillimeters);

    while (!processMovement())
        ;
}

//
// setup a move relative to the current position, units are in millimeters, no
// motion occurs until processMove() is called
//  Enter:  distanceToMoveInMillimeters = signed distance to move relative to the
//          current position in millimeters
//
void Stepper_4::setTargetPositionRelativeInMillimeters(
    float distanceToMoveInMillimeters)
{
    setTargetPositionRelativeInSteps((long)round(distanceToMoveInMillimeters *
                                                 stepsPerMillimeter));
}

//
// move to the given absolute position, units are in millimeters, this function
// does not return until the move is complete
//  Enter:  absolutePositionToMoveToInMillimeters = signed absolute position to
//          move to in units of millimeters
//
void Stepper_4::moveToPositionInMillimeters(
    float absolutePositionToMoveToInMillimeters)
{
    setTargetPositionInMillimeters(absolutePositionToMoveToInMillimeters);

    while (!processMovement())
        ;
}

//
// setup a move, units are in millimeters, no motion occurs until processMove()
// is called
//  Enter:  absolutePositionToMoveToInMillimeters = signed absolute position to
//          move to in units of millimeters
//
void Stepper_4::setTargetPositionInMillimeters(
    float absolutePositionToMoveToInMillimeters)
{
    setTargetPositionInSteps((long)round(absolutePositionToMoveToInMillimeters *
                                         stepsPerMillimeter));
}

float Stepper_4::getTargetPositionInMillimeters()
{
    return getTargetPositionInSteps() / stepsPerMillimeter;
}

//
// Get the current velocity of the motor in millimeters/second.  This functions is
// updated while it accelerates up and down in speed.  This is not the desired
// speed, but the speed the motor should be moving at the time the function is
// called.  This is a signed value and is negative when the motor is moving
// backwards.  Note: This speed will be incorrect if the desired velocity is set
// faster than this library can generate steps, or if the load on the motor is too
// great for the amount of torque that it can generate.
//  Exit:  velocity speed in steps per second returned, signed
//
float Stepper_4::getCurrentVelocityInMillimetersPerSecond()
{
    return (getCurrentVelocityInStepsPerSecond() / stepsPerMillimeter);
}

// ---------------------------------------------------------------------------------
//                     Public functions with units in revolutions
// ---------------------------------------------------------------------------------

//
// set the number of steps the motor has per revolution
//
void Stepper_4::setStepsPerRevolution(float motorStepPerRevolution)
{
    stepsPerRevolution = motorStepPerRevolution;
}

//
// get the current position of the motor in revolutions, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in revolutions returned
//
float Stepper_4::getCurrentPositionInRevolutions()
{
    return ((float)getCurrentPositionInSteps() / stepsPerRevolution);
}

//
// set the current position of the motor in revolutions, this does not move the
// Do not confuse this function with setTargetPositionInRevolutions(), it does not directly cause a motor movement per se.
// NOTE: if you called one of the move functions before (and by that setting a target position internally) you might experience that the motor starts to move after calling setCurrentPositionInRevolutions() in the case that the value of currentPositionInRevolutions is different from the target position of the stepper. If this is not intended, you should call setTargetPositionInRevolutions() with the same value as the setCurrentPositionInRevolutions() function directly before or after calling setCurrentPositionInRevolutions

void Stepper_4::setCurrentPositionInRevolutions(
    float currentPositionInRevolutions)
{
    setCurrentPositionInSteps((long)round(currentPositionInRevolutions *
                                          stepsPerRevolution));
}

//
// set the maximum speed, units in revolutions/second, this is the maximum speed
// reached while accelerating
//  Enter:  speedInRevolutionsPerSecond = speed to accelerate up to, units in
//            revolutions/second
//
void Stepper_4::setSpeedInRevolutionsPerSecond(float speedInRevolutionsPerSecond)
{
    setSpeedInStepsPerSecond(speedInRevolutionsPerSecond * stepsPerRevolution);
}

//
// set the rate of acceleration, units in revolutions/second/second
//  Enter:  accelerationInRevolutionsPerSecondPerSecond = rate of acceleration,
//          units in revolutions/second/second
//
void Stepper_4::setAccelerationInRevolutionsPerSecondPerSecond(
    float accelerationInRevolutionsPerSecondPerSecond)
{
    setAccelerationInStepsPerSecondPerSecond(
        accelerationInRevolutionsPerSecondPerSecond * stepsPerRevolution);
}

//
// set the rate of deceleration, units in revolutions/second/second
//  Enter:  decelerationInRevolutionsPerSecondPerSecond = rate of deceleration,
//          units in revolutions/second/second
//
void Stepper_4::setDecelerationInRevolutionsPerSecondPerSecond(
    float decelerationInRevolutionsPerSecondPerSecond)
{
    setDecelerationInStepsPerSecondPerSecond(
        decelerationInRevolutionsPerSecondPerSecond * stepsPerRevolution);
}

//
// home the motor by moving until the homing sensor is activated, then set the
//  position to zero, with units in revolutions
//  Enter:  directionTowardHome = 1 to move in a positive direction, -1 to move in
//            a negative directions
//          speedInRevolutionsPerSecond = speed to accelerate up to while moving
//            toward home, units in revolutions/second
//          maxDistanceToMoveInRevolutions = unsigned maximum distance to move
//            toward home before giving up
//          homeSwitchPin = pin number of the home switch, switch should be
//            configured to go low when at home
//  Exit:   true returned if successful, else false
//
bool Stepper_4::moveToHomeInRevolutions(signed char directionTowardHome,
                                        float speedInRevolutionsPerSecond, long maxDistanceToMoveInRevolutions,
                                        int homeLimitSwitchPin)
{
    return (moveToHomeInSteps(directionTowardHome,
                              speedInRevolutionsPerSecond * stepsPerRevolution,
                              maxDistanceToMoveInRevolutions * stepsPerRevolution,
                              homeLimitSwitchPin));
}

//
// move relative to the current position, units are in revolutions, this function
// does not return until the move is complete
//  Enter:  distanceToMoveInRevolutions = signed distance to move relative to the
//          current position in revolutions
//
void Stepper_4::moveRelativeInRevolutions(float distanceToMoveInRevolutions)
{
    setTargetPositionRelativeInRevolutions(distanceToMoveInRevolutions);

    while (!processMovement())
        ;
}

//
// setup a move relative to the current position, units are in revolutions, no
// motion occurs until processMove() is called
//  Enter:  distanceToMoveInRevolutions = signed distance to move relative to the
//            currentposition in revolutions
//
void Stepper_4::setTargetPositionRelativeInRevolutions(
    float distanceToMoveInRevolutions)
{
    setTargetPositionRelativeInSteps((long)round(distanceToMoveInRevolutions *
                                                 stepsPerRevolution));
}

//
// move to the given absolute position, units are in revolutions, this function
// does not return until the move is complete
//  Enter:  absolutePositionToMoveToInRevolutions = signed absolute position to
//            move to in units of revolutions
//
void Stepper_4::moveToPositionInRevolutions(
    float absolutePositionToMoveToInRevolutions)
{
    setTargetPositionInRevolutions(absolutePositionToMoveToInRevolutions);

    while (!processMovement())
        ;
}

//
// setup a move, units are in revolutions, no motion occurs until processMove()
// is called
//  Enter:  absolutePositionToMoveToInRevolutions = signed absolute position to
//          move to in units of revolutions
//
void Stepper_4::setTargetPositionInRevolutions(
    float absolutePositionToMoveToInRevolutions)
{
    setTargetPositionInSteps((long)round(absolutePositionToMoveToInRevolutions *
                                         stepsPerRevolution));
}

float Stepper_4::getTargetPositionInRevolutions()
{
    return getTargetPositionInSteps() / stepsPerRevolution;
}

//
// Get the current velocity of the motor in revolutions/second.  This functions is
// updated while it accelerates up and down in speed.  This is not the desired
// speed, but the speed the motor should be moving at the time the function is
// called.  This is a signed value and is negative when the motor is moving
// backwards.  Note: This speed will be incorrect if the desired velocity is set
// faster than this library can generate steps, or if the load on the motor is too
// great for the amount of torque that it can generate.
//  Exit:  velocity speed in steps per second returned, signed
//
float Stepper_4::getCurrentVelocityInRevolutionsPerSecond()
{
    return (getCurrentVelocityInStepsPerSecond() / stepsPerRevolution);
}

// ---------------------------------------------------------------------------------
//                        Public functions with units in steps
// ---------------------------------------------------------------------------------

//
// set the current position of the motor in steps, this does not move the motor
// currentPositionInSteps = the new position value of the motor in steps to be set internally for the current position
// Do not confuse this function with setTargetPositionInMillimeters(), it does not directly cause a motor movement per se.
// Notes:
// This function should only be called when the motor is stopped
// If you called one of the move functions before (and by that setting a target position internally) you might experience that the motor starts to move after calling setCurrentPositionInSteps() in the case that the value of currentPositionInSteps is different from the target position of the stepper. If this is not intended, you should call setTargetPositionInSteps() with the same value as the setCurrentPositionInSteps() function directly before or after calling setCurrentPositionInSteps

//
void Stepper_4::setCurrentPositionInSteps(long currentPositionInSteps)
{
    currentPosition_InSteps = currentPositionInSteps;
}

//
// get the current position of the motor in steps, this functions is updated
// while the motor moves
//  Exit:  a signed motor position in steps returned
//
long Stepper_4::getCurrentPositionInSteps()
{
    return (currentPosition_InSteps);
}

//
// set the maximum speed, units in steps/second, this is the maximum speed reached
// while accelerating
//  Enter:  speedInStepsPerSecond = speed to accelerate up to, units in steps/second
//
void Stepper_4::setSpeedInStepsPerSecond(float speedInStepsPerSecond)
{
    desiredSpeed_InStepsPerSecond = speedInStepsPerSecond;
    desiredPeriod_InUSPerStep = 1000000.0 / desiredSpeed_InStepsPerSecond;
}

//
// set the rate of acceleration, units in steps/second/second
//  Enter:  accelerationInStepsPerSecondPerSecond = rate of acceleration, units in
//          steps/second/second
//
void Stepper_4::setAccelerationInStepsPerSecondPerSecond(
    float accelerationInStepsPerSecondPerSecond)
{
    acceleration_InStepsPerSecondPerSecond = accelerationInStepsPerSecondPerSecond;
    acceleration_InStepsPerUSPerUS = acceleration_InStepsPerSecondPerSecond / 1E12;

    periodOfSlowestStep_InUS =
        1000000.0 / sqrt(2.0 * acceleration_InStepsPerSecondPerSecond);
    minimumPeriodForAStoppedMotion = periodOfSlowestStep_InUS / 2.8;
}

//
// set the rate of deceleration, units in steps/second/second
//  Enter:  decelerationInStepsPerSecondPerSecond = rate of deceleration, units in
//          steps/second/second
//
void Stepper_4::setDecelerationInStepsPerSecondPerSecond(
    float decelerationInStepsPerSecondPerSecond)
{
    deceleration_InStepsPerSecondPerSecond = decelerationInStepsPerSecondPerSecond;
    deceleration_InStepsPerUSPerUS = deceleration_InStepsPerSecondPerSecond / 1E12;
}

/**
 * set the current position as the home position (Step count = 0)
 */
void Stepper_4::setCurrentPositionAsHomeAndStop()
{
    this->isOnWayToHome = false;
    this->currentStepPeriod_InUS = 0.0;
    this->nextStepPeriod_InUS = 0.0;
    this->directionOfMotion = 0;
    this->currentPosition_InSteps = 0;
    this->targetPosition_InSteps = 0;
    this->isCurrentlyHomed = true;
}

/**
 * start jogging in the direction of home (use setDirectionToHome() to set the proper direction) until the limit switch is hit, then set the position as home
 * Warning: This function requires a limit switch to be configured otherwise the motor will never stop jogging.
 * This is a non blocking function, you need make sure Stepper_4 is started as service (use startAsService() function) or need to call the processMovement function manually in your main loop.
 */
void Stepper_4::goToLimitAndSetAsHome(callbackFunction callbackFunctionForHome, long maxDistanceToMoveInSteps)
{
    if (callbackFunctionForHome)
    {
        this->_homeReachedCallback = callbackFunctionForHome;
    }
    // the second check basically utilizes the fact the the begin and end limit switch id is 1 respectively -1 so the values are the same as the direction of the movement when the steppers moves towards of of the limits
    if (this->activeLimitSwitch == 0 || this->activeLimitSwitch != this->directionTowardsHome)
    {
        this->setTargetPositionInSteps(this->getCurrentPositionInSteps() + (this->directionTowardsHome * maxDistanceToMoveInSteps));
    }
    this->isOnWayToHome = true; // set as last action, since other functions might overwrite it
}

void Stepper_4::goToLimit(signed char direction, callbackFunction callbackFunctionForLimit)
{
    if (callbackFunctionForLimit)
    {
        this->_callbackFunctionForGoToLimit = callbackFunctionForLimit;
    }

    if (this->activeLimitSwitch == 0)
    {
        this->setTargetPositionInSteps(this->getCurrentPositionInSteps() + (this->directionTowardsHome * 2000000000));
    }
    this->isOnWayToLimit = true; // set as last action, since other functions might overwrite it
}

/**
 * register a callback function to be called whenever a movement to home has been completed (does not trigger when movement passes by the home position)
 */
void Stepper_4::registerHomeReachedCallback(callbackFunction newFunction)
{
    this->_homeReachedCallback = newFunction;
}

/**
 * register a callback function to be called whenever a target position has been reached
 */
void Stepper_4::registerTargetPositionReachedCallback(positionCallbackFunction targetPositionReachedCallbackFunction)
{
    this->_targetPositionReachedCallback = targetPositionReachedCallbackFunction;
}

/**
 * start jogging (continous movement without a fixed target position)
 * uses the currently set speed and acceleration settings
 * to stop the motion call the stopJogging function.
 * Will also stop when the external limit switch has been triggered using setLimitSwitchActive() or when the emergencyStop function is triggered
 * Warning: This function requires either a limit switch to be configured otherwise or manual trigger of the stopJogging/setTargetPositionToStop or emergencyStop function, the motor will never stop jogging
 */
void Stepper_4::startJogging(signed char direction)
{
    this->setTargetPositionInSteps(direction * 2000000000);
}

/**
 * Stop jopgging, basically an alias function for setTargetPositionToStop()
 */
void Stepper_4::stopJogging()
{
    this->setTargetPositionToStop();
}

//
// move relative to the current position, units are in steps, this function does
// not return until the move is complete
//  Enter:  distanceToMoveInSteps = signed distance to move relative to the current
//          position in steps
//
void Stepper_4::moveRelativeInSteps(long distanceToMoveInSteps)
{
    setTargetPositionRelativeInSteps(distanceToMoveInSteps);

    while (!processMovement())
        ;
}

//
// setup a move relative to the current position, units are in steps, no motion
// occurs until processMove() is called
//  Enter:  distanceToMoveInSteps = signed distance to move relative to the current
//            positionin steps
//
void Stepper_4::setTargetPositionRelativeInSteps(long distanceToMoveInSteps)
{
    setTargetPositionInSteps(currentPosition_InSteps + distanceToMoveInSteps);
}

//
// move to the given absolute position, units are in steps, this function does not
// return until the move is complete
//  Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to
//            in unitsof steps
//
void Stepper_4::moveToPositionInSteps(long absolutePositionToMoveToInSteps)
{
    setTargetPositionInSteps(absolutePositionToMoveToInSteps);

    while (!processMovement())
        ;
}

//
// setup a move, units are in steps, no motion occurs until processMove() is called
//  Enter:  absolutePositionToMoveToInSteps = signed absolute position to move to
//            in units of steps
//
void Stepper_4::setTargetPositionInSteps(long absolutePositionToMoveToInSteps)
{
    // abort potentially running homing movement
    this->isOnWayToHome = false;
    this->isOnWayToLimit = false;
    targetPosition_InSteps = absolutePositionToMoveToInSteps;
    this->firstProcessingAfterTargetReached = true;
}

long Stepper_4::getTargetPositionInSteps()
{
    return targetPosition_InSteps;
}

//
// setup a "Stop" to begin the process of decelerating from the current velocity
// to zero, decelerating requires calls to processMove() until the move is complete
// Note: This function can be used to stop a motion initiated in units of steps
// or revolutions
//
void Stepper_4::setTargetPositionToStop()
{
    // abort potentially running homing movement
    this->isOnWayToHome = false;
    this->isOnWayToLimit = false;

    if (directionOfMotion == 0)
    {
        return;
    }

    long decelerationDistance_InSteps;

    //
    // move the target position so that the motor will begin deceleration now
    //
    decelerationDistance_InSteps = (long)round(
        5E11 / (deceleration_InStepsPerSecondPerSecond * currentStepPeriod_InUS *
                currentStepPeriod_InUS));

    if (directionOfMotion > 0)
        setTargetPositionInSteps(currentPosition_InSteps + decelerationDistance_InSteps);
    else
        setTargetPositionInSteps(currentPosition_InSteps - decelerationDistance_InSteps);
}

//
// if it is time, move one step
//  Exit:  true returned if movement complete, false returned not a final target
//           position yet
//
bool Stepper_4::processMovement(void)
{

    long distanceToTarget_Signed;

    unsigned long currentTime_InUS;
    unsigned long periodSinceLastStep_InUS;

    //
    // check if currently stopped
    //
    if (directionOfMotion == 0)
    {
        distanceToTarget_Signed = targetPosition_InSteps - currentPosition_InSteps;
        // check if target position in a positive direction
        if (distanceToTarget_Signed > 0)
        {
            directionOfMotion = 1;
            nextStepPeriod_InUS = periodOfSlowestStep_InUS;
            lastStepTime_InUS = micros();
            lastStepDirectionBeforeLimitSwitchTrigger = directionOfMotion;
            return (false);
        }

        // check if target position in a negative direction
        else if (distanceToTarget_Signed < 0)
        {
            directionOfMotion = -1;
            nextStepPeriod_InUS = periodOfSlowestStep_InUS;
            lastStepTime_InUS = micros();
            lastStepDirectionBeforeLimitSwitchTrigger = directionOfMotion;
            return (false);
        }
        else
        {
            this->lastStepDirectionBeforeLimitSwitchTrigger = 0;
            return (true);
        }
    }

    // determine how much time has elapsed since the last step (Note 1: this method
    // works even if the time has wrapped. Note 2: all variables must be unsigned)
    currentTime_InUS = micros();
    periodSinceLastStep_InUS = currentTime_InUS - lastStepTime_InUS;
    // if it is not time for the next step, return
    if (periodSinceLastStep_InUS < (unsigned long)nextStepPeriod_InUS)
        return (false);

    // execute the step on the rising edge
    if (directionOfMotion == 1)
    {
        stepPos++;
        if (stepPos > 3)
            stepPos = 0;
        digitalWrite(this->SS, LOW);
        SPI.transfer(stepSeq[stepPos]);
        digitalWrite(SS, HIGH);
    }
    else if (directionOfMotion == -1)
    {
        stepPos--;
        if (stepPos < 0)
            stepPos = 4;
        digitalWrite(this->SS, LOW);
        SPI.transfer(stepSeq[stepPos]);
        digitalWrite(SS, HIGH);
    }

    // update the current position and speed
    currentPosition_InSteps += directionOfMotion;
    currentStepPeriod_InUS = nextStepPeriod_InUS;

    // remember the time that this step occured
    lastStepTime_InUS = currentTime_InUS;

    // figure out how long before the next step
    DeterminePeriodOfNextStep();

    // check if the move has reached its final target position, return true if all
    // done
    if (currentPosition_InSteps == targetPosition_InSteps)
    {
        // at final position, make sure the motor is not going too fast
        if (nextStepPeriod_InUS >= minimumPeriodForAStoppedMotion)
        {
            currentStepPeriod_InUS = 0.0;
            nextStepPeriod_InUS = 0.0;
            directionOfMotion = 0;
            this->lastStepDirectionBeforeLimitSwitchTrigger = 0;

            if (this->firstProcessingAfterTargetReached)
            {
                firstProcessingAfterTargetReached = false;
                if (this->_targetPositionReachedCallback)
                {
                    this->_targetPositionReachedCallback(currentPosition_InSteps);
                }
            }
            return (true);
        }
    }
    return (false);
}

// Get the current velocity of the motor in steps/second.  This functions is
// updated while it accelerates up and down in speed.  This is not the desired
// speed, but the speed the motor should be moving at the time the function is
// called.  This is a signed value and is negative when the motor is moving
// backwards.  Note: This speed will be incorrect if the desired velocity is set
// faster than this library can generate steps, or if the load on the motor is too
// great for the amount of torque that it can generate.
//  Exit:  velocity speed in steps per second returned, signed
//
float Stepper_4::getCurrentVelocityInStepsPerSecond()
{
    if (currentStepPeriod_InUS == 0.0)
        return (0);
    else
    {
        if (directionOfMotion > 0)
            return (1000000.0 / currentStepPeriod_InUS);
        else
            return (-1000000.0 / currentStepPeriod_InUS);
    }
}

//
// check if the motor has competed its move to the target position
//  Exit:  true returned if the stepper is at the target position
//
bool Stepper_4::motionComplete()
{
    if ((directionOfMotion == 0) &&
        (currentPosition_InSteps == targetPosition_InSteps))
        return (true);
    else
        return (false);
}

//
// determine the period for the next step, either speed up a little, slow down a
// little or go the same speed
//
void Stepper_4::DeterminePeriodOfNextStep()
{
    long distanceToTarget_Signed;
    long distanceToTarget_Unsigned;
    long decelerationDistance_InSteps;
    float currentStepPeriodSquared;
    bool speedUpFlag = false;
    bool slowDownFlag = false;
    bool targetInPositiveDirectionFlag = false;
    bool targetInNegativeDirectionFlag = false;

    //
    // determine the distance to the target position
    //
    distanceToTarget_Signed = targetPosition_InSteps - currentPosition_InSteps;
    if (distanceToTarget_Signed >= 0L)
    {
        distanceToTarget_Unsigned = distanceToTarget_Signed;
        targetInPositiveDirectionFlag = true;
    }
    else
    {
        distanceToTarget_Unsigned = -distanceToTarget_Signed;
        targetInNegativeDirectionFlag = true;
    }

    //
    // determine the number of steps needed to go from the current speed down to a
    // velocity of 0, Steps = Velocity^2 / (2 * Deceleration)
    //
    currentStepPeriodSquared = currentStepPeriod_InUS * currentStepPeriod_InUS;
    decelerationDistance_InSteps = (long)round(
        5E11 / (deceleration_InStepsPerSecondPerSecond * currentStepPeriodSquared));

    //
    // check if: Moving in a positive direction & Moving toward the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed > 0)
    //
    if ((directionOfMotion == 1) && (targetInPositiveDirectionFlag))
    {
        //
        // check if need to start slowing down as we reach the target, or if we
        // need to slow down because we are going too fast
        //
        if ((distanceToTarget_Unsigned < decelerationDistance_InSteps) ||
            (nextStepPeriod_InUS < desiredPeriod_InUSPerStep))
            slowDownFlag = true;
        else
            speedUpFlag = true;
    }

    //
    // check if: Moving in a positive direction & Moving away from the target
    //    (directionOfMotion == 1) && (distanceToTarget_Signed < 0)
    //
    else if ((directionOfMotion == 1) && (targetInNegativeDirectionFlag))
    {
        //
        // need to slow down, then reverse direction
        //
        if (currentStepPeriod_InUS < periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            directionOfMotion = -1;
        }
    }

    //
    // check if: Moving in a negative direction & Moving toward the target
    //    (directionOfMotion == -1) && (distanceToTarget_Signed < 0)
    //
    else if ((directionOfMotion == -1) && (targetInNegativeDirectionFlag))
    {
        //
        // check if need to start slowing down as we reach the target, or if we
        // need to slow down because we are going too fast
        //
        if ((distanceToTarget_Unsigned < decelerationDistance_InSteps) ||
            (nextStepPeriod_InUS < desiredPeriod_InUSPerStep))
            slowDownFlag = true;
        else
            speedUpFlag = true;
    }

    //
    // check if: Moving in a negative direction & Moving away from the target
    //    (directionOfMotion == -1) && (distanceToTarget_Signed > 0)
    //
    else if ((directionOfMotion == -1) && (targetInPositiveDirectionFlag))
    {
        //
        // need to slow down, then reverse direction
        //
        if (currentStepPeriod_InUS < periodOfSlowestStep_InUS)
        {
            slowDownFlag = true;
        }
        else
        {
            directionOfMotion = 1;
        }
    }

    //
    // check if accelerating
    //
    if (speedUpFlag)
    {
        //
        // StepPeriod = StepPeriod(1 - a * StepPeriod^2)
        //
        nextStepPeriod_InUS = currentStepPeriod_InUS - acceleration_InStepsPerUSPerUS *
                                                           currentStepPeriodSquared * currentStepPeriod_InUS;

        if (nextStepPeriod_InUS < desiredPeriod_InUSPerStep)
            nextStepPeriod_InUS = desiredPeriod_InUSPerStep;
    }

    //
    // check if decelerating
    //
    if (slowDownFlag)
    {
        //
        // StepPeriod = StepPeriod(1 + a * StepPeriod^2)
        //
        nextStepPeriod_InUS = currentStepPeriod_InUS + deceleration_InStepsPerUSPerUS *
                                                           currentStepPeriodSquared * currentStepPeriod_InUS;

        if (nextStepPeriod_InUS > periodOfSlowestStep_InUS)
            nextStepPeriod_InUS = periodOfSlowestStep_InUS;
    }
}

// -------------------------------------- End --------------------------------------
