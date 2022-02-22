#ifndef V_I_CARD_H
#define V_I_CARD_H

#include "Card.h"
#include <Arduino.h>
#include "board.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

class V_I_Card : public Card
{
public:
    V_I_Card(int addr_to_use = 0);
    void measureAndReturn(byte channel, String *text);
    void measureAndReturnRAW(byte channel, String *text);
    void Start();
    String getStatus() const;
    String ReturnGains();
    void LoadGains(int channel, int setting, float value);
    ~V_I_Card();

private:
    void readWithRange(int channel, float *result, int *gain_s);
    void readWithGain(int channel, float *result, int gain_s, int16_t *raw);
    const adsGain_t gains[6] = {GAIN_TWOTHIRDS, GAIN_ONE, GAIN_TWO, GAIN_FOUR, GAIN_EIGHT, GAIN_SIXTEEN};
    const int16_t ranges[6] = {21845, 16384, 16384, 16384, 16384, 16384};
    float voltageInputMultiplier[6][4];
    bool adsStatus[2];
    Adafruit_ADS1115 ads[2];
    int addr;
    const char sys[9] = "V_I_Card";
};

#endif