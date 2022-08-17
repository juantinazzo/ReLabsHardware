#ifndef MASS_SELECTOR_H
#define MASS_SELECTOR_H

#include <Arduino.h>
#include "board.h"
#include <ESP32Servo.h>

class Mass_Selector
{
public:
    Mass_Selector();
    Mass_Selector(uint8_t servo_SLOT_p);
    void reset();
    void goTo(uint8_t mass_index_p);
    bool isMoving();
    uint8_t currentMass();

private:
    uint8_t mass_index;
    bool status;
    const char sys[14] = "Mass_Selector";
    bool is_moving = 0;
};
#endif