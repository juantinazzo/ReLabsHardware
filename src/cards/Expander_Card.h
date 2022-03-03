#ifndef EXPANDER_CARD_H
#define EXPANDER_CARD_H

#include <Arduino.h>
#include "Card.h"
#include "board.h"
#include <Wire.h>

#define MCP23016_GPIO0 0x00   // Data port register 0
#define MCP23016_GPIO1 0x01   // Data port register 1
#define MCP23016_OLAT0 0x02   // Access output latch register 0
#define MCP23016_OLAT1 0x03   // Access output latch register 1
#define MCP23016_IPOL0 0x04   // Access input polarity port register 0
#define MCP23016_IPOL1 0x05   // Access input polarity port register 1
#define MCP23016_IODIR0 0x06  // I/O direction register 0
#define MCP23016_IODIR1 0x07  // I/O direction register 1
#define MCP23016_INTCAP0 0x08 // Interrupt capture 0
#define MCP23016_INTCAP1 0x09 // Interrupt capture 1
#define MCP23016_IOCON0 0x0A  // I/O Expander control register 0
#define MCP23016_IOCON1 0x0B  // I/O Expander control register 1

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