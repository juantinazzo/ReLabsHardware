#ifndef MASS_SELECTOR_H
#define MASS_SELECTOR_H

#include <Arduino.h>
#include "board.h"
#include <ESP32Servo.h>

#define SPEED_IN_STEPS_PER_SECOND 300
#define ACCELERATION_IN_STEPS_PER_SECOND 800
#define DECELERATION_IN_STEPS_PER_SECOND 800
#define MOTOR_STEPS_PER_REVOLUTION 1000
#define homingDir 1

class Mass_Selector
{
public:
    Mass_Selector();
    Mass_Selector(uint8_t SLOT_p);
    void reset();
    void goTo(uint8_t mass_index_p);
    bool isMoving();
    uint8_t currentMass();

private:
    uint8_t mass_index, SLOT;
    bool status;
    const char sys[14] = "Mass_Selector";
    bool is_moving = 0;
};
#endif