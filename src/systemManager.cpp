#include <Arduino.h>
#include "cards/V_I_Card.h"
#include "cards/V_O_Card.h"
#include "utilities/Logger.h"
#include "systemManager.h"
#include "hardware_libs/IO_expander.h"

static char sys[] = "systemManager";

systemManager::systemManager()
{
}

bool systemManager::startVO(uint8_t SLOT)
{
    if (VO_index > 1)
        return false;
    VO[VO_index].setSlot(SLOT);
    VO[VO_index].start();
    if (VO[VO_index].isRunning())
        VO_index++;
    return true;
};
bool systemManager::startVI(uint8_t addr)
{
    if (VI_index > 1)
        return false;
    VI[VI_index].setADDR(addr);
    VI[VI_index].start();
    if (VI[VI_index].isRunning())
        VI_index++;
    return true;
}
bool systemManager::startIOExp()
{
    startExpanders();
    return true;
}

systemManager::~systemManager()
{
}