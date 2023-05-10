#include <Arduino.h>
#include "Mass_Lifter.h"
#include "board.h"
#include "utilities/Logger.h"
#include <ESP_FlexyStepper.h>
#include <ESP32Servo.h>

ESP_FlexyStepper stepper_l;
Servo servo_l;

Mass_Lifter::Mass_Lifter()
{
}

Mass_Lifter::Mass_Lifter(uint8_t stepper_dir_pin_p, uint8_t stepper_step_pin_p, uint8_t servo_pin_p)
{
    servo_pin = servo_pin_p;
    stepper_dir_pin = stepper_dir_pin_p;
    stepper_step_pin = stepper_step_pin_p;

    stepper_l.connectToPins(stepper_step_pin, stepper_dir_pin);
    stepper_l.setSpeedInStepsPerSecond(SPEED_IN_STEPS_PER_SECOND);
    stepper_l.setAccelerationInStepsPerSecondPerSecond(ACCELERATION_IN_STEPS_PER_SECOND);
    stepper_l.setDecelerationInStepsPerSecondPerSecond(DECELERATION_IN_STEPS_PER_SECOND);
    stepper_l.setStepsPerRevolution(MOTOR_STEPS_PER_REVOLUTION);
    stepper_l.setDirectionToHome(homingDir);
    stepper_l.startAsService(1);

    servo_l.attach(servo_pin, minUs, maxUs);
}

float Mass_Lifter::getAngle()
{
    return stepper_l.getCurrentPositionInRevolutions();
}

bool Mass_Lifter::isMoving()
{
    return ~stepper_l.motionComplete();
}

bool Mass_Lifter::home()
{
    stepper_l.startJogging(homingDir);
    stepper_l.goToLimitAndSetAsHome();
    return true;
}

void Mass_Lifter::goToAngle(uint8_t angle_deg_p)
{
    stepper_l.setTargetPositionInRevolutions(angle_deg_p);
}

void Mass_Lifter::release()
{
    servo_l.write(releaseAngle);
}

void Mass_Lifter::deploy()
{
    servo_l.write(deployAngle);
}