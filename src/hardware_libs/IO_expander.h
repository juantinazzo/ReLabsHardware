#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H

#include <Arduino.h>
#include <Wire.h>

void configureExpander(byte number, uint16_t pinMode);
void setExpanderOutputs(byte number, uint16_t values);
uint16_t readExpanderInputs(byte number);

byte getExpanderAddress(byte number);
bool checkExpanderStatus(byte number);
String printStatusExpander();
void startExpanders();

extern bool expanderStatus[8];

#endif