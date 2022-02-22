#ifndef VOLTAGE_OUTPUTS_H
#define VOLTAGE_OUTPUTS_H

#include <Arduino.h>
#include <MCP492X.h>
#include "board.h"

#define PIN_SPI_CHIP_SELECT_DAC0 SLOT1_CS0
#define PIN_SPI_CHIP_SELECT_DAC1 SLOT1_CS1

void startVoltageOutputs();
void setChannelVoltage(char channel, float mV);
uint16_t voltageToValue(char channel, float mV);

extern bool voltageOutputsStatus[4];
extern float voltageOutputMultiplier[4];
extern uint16_t voltageOutputOffset[4];

#endif