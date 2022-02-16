#include <Arduino.h>
#include "Voltage_Outputs.h"

MCP492X voltageDAC0(PIN_SPI_CHIP_SELECT_DAC0), voltageDAC1(PIN_SPI_CHIP_SELECT_DAC1);

void startVoltageOutputs()
{
    voltageDAC0.begin();
    voltageDAC1.begin();
    for (char i = 0; i < 4; i++)
        setChannelVoltage(i, 0);
}

void setChannelVoltage(char channel, float mV)
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

u_int16_t voltageToValue(char channel, float mV)
{
    return mV * voltageOutputMultiplier[channel] + voltageOutputOffset[channel];
}