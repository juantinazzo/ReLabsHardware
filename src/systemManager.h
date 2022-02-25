#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <Arduino.h>
#include "cards/V_I_Card.h"
#include "cards/V_O_Card.h"
#include "cards/I_O_Card.h"
#include "cards/Servo_Card.h"
#include "cards/Expander_Card.h"

class systemManager
{
public:
    systemManager();
    bool startVO(uint8_t SLOT);
    bool startVI(uint8_t addr);
    bool startIO(uint8_t SLOT);
    bool startSERVO(uint8_t output_pin);
    bool startEXP(uint8_t number);
    ~systemManager();
    V_I_Card VI[3];
    V_O_Card VO[2];
    I_O_Card IO[6];
    Servo_Card SERVO[3];
    Expander_Card EXP[8];

private:
    uint8_t VO_index = 0;
    uint8_t VI_index = 0;
    uint8_t IO_index = 0;
    uint8_t SERVO_index = 0;
    uint8_t EXP_index = 0;
};

#endif