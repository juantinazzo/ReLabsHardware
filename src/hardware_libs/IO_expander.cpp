#include <Arduino.h>
#include "IO_expander.h"

void configureExpander(byte number, uint16_t pinMode)
{
    byte addr = getExpanderAddress(number);
    Wire.beginTransmission(addr);      // setup out direction registers
    Wire.write(0x06);                  // pointer
    Wire.write(0xFF & pinMode);        // DDR Port0 all output
    Wire.write(0xFF & (pinMode >> 8)); // DDR Port1 all input
    Wire.endTransmission();
}

void setExpanderOutputs(byte number, uint16_t values)
{
    byte addr = getExpanderAddress(number);
    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.write(values & 0xFF);
    Wire.write(values >> 8);
    Wire.endTransmission();
}
uint16_t readExpanderInputs(byte number)
{
    uint16_t bothPorts = 0;
    uint8_t port0;
    byte addr = getExpanderAddress(number);
    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(addr, 2);
    port0 = Wire.read();
    bothPorts = Wire.read();
    bothPorts <<= 8;
    bothPorts |= port0;
    return bothPorts;
}

byte getExpanderAddress(byte number)
{
    return number + 0x20;
}

bool checkExpanderStatus(byte number)
{
    byte addr = getExpanderAddress(number);
    Wire.beginTransmission(addr);
    return (Wire.endTransmission() == 0);
}

void startExpanders()
{
    for (byte i = 0; i < 8; i++)
    {
        expanderStatus[i] = checkExpanderStatus(i);
        if (expanderStatus[i])
        {
            configureExpander(i, 0xFFFF);
        }
    }
}

String printStatusExpander()
{
    String res = "{ \"hardware\": expander, ";
    Serial.print("\n EXPANDER STATUS:\n\t");
    for (byte i = 0; i < 8; i++)
    {
        Serial.print("expander");
        Serial.print(i);
        Serial.print(expanderStatus[i] ? ": Active\n\t" : ": Inactive\n\t");
        res += "\"" + String(i) + "\": ";
        res += expanderStatus[i] ? "1" : "0";
        if (i != 7)
            res += ", ";
    }
    res += "}";
    Serial.print("\n");

    return res;
}