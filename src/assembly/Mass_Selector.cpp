#include <Arduino.h>
#include "Mass_Selector.h"
#include "board.h"
#include "utilities/Logger.h"
#include "hardware_libs/Stepper_4.h"

Stepper_4 stepper4;

Mass_Selector::Mass_Selector()
{
}

Mass_Selector::Mass_Selector(uint8_t SLOT_p)
{
    SLOT = SLOT_p;
    stepper4.setSS(SLOT);
    stepper4.setSpeedInStepsPerSecond(SPEED_IN_STEPS_PER_SECOND);
    stepper4.setAccelerationInStepsPerSecondPerSecond(ACCELERATION_IN_STEPS_PER_SECOND);
    stepper4.setDecelerationInStepsPerSecondPerSecond(DECELERATION_IN_STEPS_PER_SECOND);
    stepper4.setStepsPerRevolution(MOTOR_STEPS_PER_REVOLUTION);
    stepper4.setDirectionToHome(homingDir);
    stepper4.startAsService(1);
}