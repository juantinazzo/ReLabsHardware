#include <Arduino.h>
#include "V_O_Card.h"
#include "board.h"
#include "hardware_libs/MCP4922.h"
#include "utilities/Logger.h"

MCP4922 voltageDAC0, voltageDAC1;

V_O_Card::V_O_Card()
{
}

void V_O_Card::setSlot(uint8_t SLOT)
{
    SLOT_p = SLOT;
}

V_O_Card::V_O_Card(uint8_t SLOT)
{
    SLOT_p = SLOT;
    start();
}

void V_O_Card::start()
{
    switch (SLOT_p)
    {
    case 0:
        voltageDAC0.begin(SLOT0_CS0);
        voltageDAC1.begin(SLOT0_CS1);
        break;
    case 1:
        voltageDAC0.begin(SLOT1_CS0);
        voltageDAC1.begin(SLOT1_CS1);
        break;
    default:
        LOG("VO%d not started. Wrong SLOT", Error, sys, SLOT_p);
        return;
    }

    for (char i = 0; i < 4; i++)
        setChannelVoltage(i, 0);

    LOG("VO%d started", Info, sys, SLOT_p);
    is_running = true;
}

void V_O_Card::setChannelVoltage(char channel, float mV)
{
    switch (channel)
    {
    case 0:
        voltageDAC0.analogWrite(0, 1, 1, 1, voltageToValue(channel, mV));
        break;
    case 1:
        voltageDAC0.analogWrite(1, 1, 1, 1, voltageToValue(channel, mV));
        break;
    case 2:
        voltageDAC1.analogWrite(0, 1, 1, 1, voltageToValue(channel, mV));
        break;
    case 3:
        voltageDAC1.analogWrite(1, 1, 1, 1, voltageToValue(channel, mV));
        break;
    default:
        break;
    }
}

uint16_t V_O_Card::voltageToValue(char channel, float mV)
{
    return mV * V_O_GO[channel].getGain() + V_O_GO[channel].getOffset();
}

bool V_O_Card::isRunning()
{
    return is_running;
}