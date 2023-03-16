#ifndef I_O_CARD_H
#define I_O_CARD_H

#include <Arduino.h>
#include "Card.h"
#include "board.h"
#include "utilities/ioOffsetGains.h"

class I_O_Card : public Card
{
public:
    I_O_Card();
    I_O_Card(uint8_t SLOT);
    void start();
    void setChannelCurrent(uint8_t channel, float mA);
    bool isRunning();
    void setSlot(uint8_t SLOT);

private:
    uint8_t SLOT_p;
    uint16_t currentToValue(uint8_t channel, float mA);
    bool status[2];
    ioOffsetGains I_O_GO[2];
    const char sys[9] = "I_O_Card";
    bool is_running = 0;
};
#endif