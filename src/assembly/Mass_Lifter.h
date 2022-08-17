#ifndef MASS_LIFTER_H
#define MASS_LIFTER_H

#include <Arduino.h>
#include "board.h"
#include <ESP32Servo.h>

#define SPEED_IN_STEPS_PER_SECOND 300
#define ACCELERATION_IN_STEPS_PER_SECOND 800
#define DECELERATION_IN_STEPS_PER_SECOND 800
#define MOTOR_STEPS_PER_REVOLUTION 1000
#define minUs 1000
#define maxUs 2000
#define releaseAngle 0
#define deployAngle 90
#define homingDir 1

class Mass_Lifter
{
public:
    Mass_Lifter();
    Mass_Lifter(uint8_t stepper_dir_pin_p, uint8_t stepper_step_pin_p, uint8_t servo_pin_p);
    void goToAngle(uint8_t angle_deg_p);
    float getAngle();
    bool isMoving();
    uint8_t currentMass();
    bool home();
    void release();
    void deploy();

private:
    uint8_t servo_pin, stepper_dir_pin, stepper_step_pin;
    bool status;
    const char sys[14] = "Mass_Lifter";
    bool is_moving = 0;
};
#endif