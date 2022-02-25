#include <Arduino.h>
#include "cards/V_I_Card.h"
#include "cards/V_O_Card.h"
#include "utilities/Logger.h"
#include "systemManager.h"

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

bool systemManager::startIO(uint8_t SLOT)
{
    if (IO_index > 5)
        return false;
    IO[VO_index].setSlot(SLOT);
    IO[VO_index].start();
    if (IO[IO_index].isRunning())
        IO_index++;
    else
        return false;
    return true;
};

bool systemManager::startSERVO(uint8_t output_pin)
{
    if (SERVO_index > 2)
        return false;
    SERVO[SERVO_index].setOutputPin(output_pin);
    SERVO[SERVO_index].start();
    if (SERVO[SERVO_index].isRunning())
        SERVO_index++;
    else
        return false;
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
bool systemManager::startEXP(uint8_t number)
{
    if (EXP_index > 7)
        return false;
    EXP[EXP_index].setNumber(number);
    EXP[EXP_index].start();
    if (EXP[EXP_index].isRunning())
        EXP_index++;
    else
        return false;
    return true;
}

systemManager::~systemManager()
{
}