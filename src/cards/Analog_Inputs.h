#include <Arduino.h>
#include "board.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

void measureAndReturn(byte channel, String *text);
void measureAndReturnRAW(byte channel, String *text);
void readWithRange(int channel, float *result, int *gain_s);
void readWithGain(int channel, float *result, int gain_s, int16_t *raw);
void startAnalogInputs();
String ReturnAnalogInputGains();
void LoadAnalogInputGains(int channel, int setting, float value);
String printStatusADS();

const adsGain_t gains[] = {GAIN_TWOTHIRDS, GAIN_ONE, GAIN_TWO, GAIN_FOUR, GAIN_EIGHT, GAIN_SIXTEEN};
const int16_t ranges[] = {21845, 16384, 16384, 16384, 16384, 16384};
extern float voltageInputMultiplier[6][8];
extern bool adsStatus[4];

