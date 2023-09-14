#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

typedef struct{
    uint8_t _step,
    uint8_t _dir,
    int32_t _abs_pos
} motor_t;

void motor_driver_start(uint8_t step, uint8_t dir);

#endif