#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <Arduino.h>
#include "cards/V_I_Card.h"
#include "cards/V_O_Card.h"

class systemManager
{
public:
    systemManager();
    bool startVO(uint8_t SLOT);
    bool startVI(uint8_t addr);
    bool startIOExp();
    ~systemManager();
    V_I_Card VI[3];
    V_O_Card VO[2];

private:
    uint8_t VO_index = 0;
    uint8_t VI_index = 0;
};

#endif