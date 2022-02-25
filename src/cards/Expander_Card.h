#ifndef EXPANDER_CARD_H
#define EXPANDER_CARD_H

#include <Arduino.h>
#include "Card.h"
#include "board.h"
#include <Wire.h>

class Expander_Card : public Card
{
public:
    Expander_Card();
    Expander_Card(uint8_t number);
    void setNumber(uint8_t number);
    void start();
    void configure(uint16_t pinMode);
    bool isRunning();
    void setOutputPin(uint8_t output_pin, bool value);
    void setOutputs(uint16_t values);
    bool readInputPin(uint8_t input_pin);
    uint16_t readInputs();

private:
    byte getExpanderAddress(byte number);
    uint8_t addr;
    bool status;
    const char sys[14] = "Expander_Card";
    bool is_running = 0;
    uint16_t lastOutput;
};
#endif