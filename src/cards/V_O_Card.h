#ifndef V_O_CARD_H
#define V_O_CARD_H

#include <Arduino.h>
#include "Card.h"
#include <MCP492X.h>
#include "board.h"
#include "utilities/ioOffsetGains.h"

class V_O_Card : public Card
{
public:
    V_O_Card();
    V_O_Card(uint8_t SLOT);
    void start();
    void setChannelVoltage(uint8_t channel, float mV);
    bool isRunning();
    void setSlot(uint8_t SLOT);

private:
    uint8_t SLOT_p;
    uint16_t voltageToValue(uint8_t channel, float mV);
    bool status[4];
    ioOffsetGains V_O_GO[4];
    const char sys[9] = "V_O_Card";
    bool is_running = 0;
};
#endif