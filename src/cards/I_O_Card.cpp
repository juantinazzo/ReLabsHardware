#include <Arduino.h>
#include "I_O_Card.h"
#include "board.h"
#include "hardware_libs/MCP4922.h"
#include "utilities/Logger.h"

MCP4922 currentDAC;

I_O_Card::I_O_Card()
{
}

void I_O_Card::setSlot(uint8_t SLOT)
{
    SLOT_p = SLOT;
}

I_O_Card::I_O_Card(uint8_t SLOT)
{
    SLOT_p = SLOT;
    start();
}

void I_O_Card::start()
{
    switch (SLOT_p)
    {
    case 0:
        currentDAC.begin(SLOT0_CS0);
        break;
    case 1:
        currentDAC.begin(SLOT1_CS0);
        break;
    case 2:
        currentDAC.begin(SLOT2_CS0);
        break;
    case 3:
        currentDAC.begin(SLOT3_CS0);
        break;
    case 4:
        currentDAC.begin(SLOT4_CS0);
        break;
    case 5:
        currentDAC.begin(SLOT5_CS0);
        break;
    default:
        LOG("VO%d not started. Wrong SLOT", Error, sys, SLOT_p);
        return;
    }

    for (char i = 0; i < 2; i++)
        setChannelCurrent(i, 0);

    LOG("IO%d started", Info, sys, SLOT_p);
    is_running = true;
}

void I_O_Card::setChannelCurrent(uint8_t channel, float mA)
{
    switch (channel)
    {
    case 0:
        currentDAC.analogWrite(0, 1, 1, 1, currentToValue(channel, mA));
        break;
    case 1:
        currentDAC.analogWrite(1, 1, 1, 1, currentToValue(channel, mA));
        break;
    default:
        break;
    }
}

uint16_t I_O_Card::currentToValue(uint8_t channel, float mA)
{
    return mA * I_O_GO[channel].getGain() + I_O_GO[channel].getOffset();
}

bool I_O_Card::isRunning()
{
    return is_running;
}