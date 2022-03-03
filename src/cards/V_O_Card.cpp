#include <Arduino.h>
#include "V_O_Card.h"
#include "board.h"
#include "hardware_libs/MCP4922.h"
#include "utilities/Logger.h"
#include "utilities/ConfigSaver.h"

#define FOR4(var) for (uint8_t var; var < 4; var++)

MCP4922 voltageDAC0, voltageDAC1;
extern ConfigSaver CS;
char temp[30];

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
       // FOR4(i)
        //CS.getGainOffset(&V_O_GO[i], joinName("V_O_Card_", 0, i));
        break;
    case 1:
        voltageDAC0.begin(SLOT1_CS0);
        voltageDAC1.begin(SLOT1_CS1);
       // FOR4(i)
        //CS.getGainOffset(&V_O_GO[i], joinName("V_O_Card_", 1, i));
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

void V_O_Card::setChannelVoltage(uint8_t channel, float mV)
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

uint16_t V_O_Card::voltageToValue(uint8_t channel, float mV)
{
    return mV * V_O_GO[channel].getGain() + V_O_GO[channel].getOffset();
}

bool V_O_Card::isRunning()
{
    return is_running;
}

void V_O_Card::setGainOffset(uint8_t channel, float gain, float offset)
{
    V_O_GO[channel].setGain(gain);
    V_O_GO[channel].setOffset(offset);
   // CS.setGainOffset(&V_O_GO[channel], joinName("V_O_Card_", SLOT_p, channel));
}

char *V_O_Card::joinName(char *base, uint8_t slot, uint8_t channel)
{
    sprintf(temp, "%s%d%d", base, slot, channel);
    return temp;
}