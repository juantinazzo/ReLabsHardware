#include <Arduino.h>
#include "Expander_Card.h"
#include "board.h"
#include "utilities/Logger.h"

Expander_Card::Expander_Card()
{
}

void Expander_Card::setNumber(uint8_t number)
{
    addr = getExpanderAddress(number);
}

Expander_Card::Expander_Card(uint8_t number)
{
    addr = getExpanderAddress(number);
    start();
}

void Expander_Card::start()
{
    if (isRunning())
        LOG("Expander at address %X started", Info, sys, addr);
    else
        LOG("Expander at address %X failed to start", Error, sys, addr);
}

bool Expander_Card::isRunning()
{
    Wire.beginTransmission(addr);
    is_running = (Wire.endTransmission() == 0);
    return is_running;
}

void Expander_Card::configure(uint16_t pinMode)
{
    Wire.beginTransmission(addr);
    Wire.write(0x06);
    Wire.write(0xFF & pinMode);
    Wire.write(0xFF & (pinMode >> 8));
    Wire.endTransmission();
}

void Expander_Card::setOutputs(uint16_t values)
{
    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.write(values & 0xFF);
    Wire.write(values >> 8);
    Wire.endTransmission();
    lastOutput = values;
}

void Expander_Card::setOutputPin(uint8_t output_pin, bool value)
{
    if (value)
        setOutputs(lastOutput | (1 << output_pin));
    else
        setOutputs(lastOutput & (~(1 << output_pin)));
}

uint16_t Expander_Card::readInputs()
{
    uint16_t bothPorts = 0;
    uint8_t port0;
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

bool Expander_Card::readInputPin(uint8_t input_pin)
{
    return (readInputs() >> input_pin) & 1;
}

byte Expander_Card::getExpanderAddress(byte number)
{
    return number + 0x20;
}

/*String printStatusExpander()
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
}*/