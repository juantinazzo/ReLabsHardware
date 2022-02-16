#include <Arduino.h>
#include "Analog_Inputs.h"

Adafruit_ADS1115 ads[4];

void measureAndReturn(byte channel, String *text)
{
    digitalWrite(led, LOW);
    String valueStr;
    String contentJson;
    if (adsStatus[channel / 2])
    {
        float results;
        int gain_s;
        readWithRange(channel, &results, &gain_s);
        valueStr = String(results);
        contentJson = "\"value\":" + String(results, 6) + ", \"unit\":\"mV\", \"gain\":" + String(gain_s, DEC) + ", \"channel\":" + String(channel, DEC) + ",\"status\": \"ok\"";
        //Serial.println(valueStr);
    }
    else
    {
        contentJson = "\"channel\":" + String(channel) + ", \"status\": \"error\"";
    }

    digitalWrite(led, HIGH);
    *text = contentJson;
}

void measureAndReturnRAW(byte channel, String *text)
{
    digitalWrite(led, LOW);
    String valueStr;
    if (adsStatus[channel / 2])
    {
        float results;
        int16_t raw[6];

        valueStr += "\"channel\":" + String(channel) + ",";

        for (int i = 0; i < 6; i++)
        {
            readWithGain(channel, &results, i, raw + i);
            valueStr += "\"";
            valueStr += String(i);
            valueStr += "\":";
            valueStr += String(raw[i]);
            valueStr += ",";
        }
        valueStr += "\"status\": \"ok\"";
    }
    else
    {
        valueStr = "\"channel\":" + String(channel) + ", \"status\": \"error\"";
    }

    *text = valueStr;

    digitalWrite(led, HIGH);
}

void readWithRange(int channel, float *result, int *gain_s)
{
    int16_t readVal;
    *gain_s = -1;
    do
    {
        *gain_s += 1;
        ads[channel / 2].setGain(gains[*gain_s]);
        readVal = channel % 2 == 0 ? ads[channel / 2].readADC_Differential_0_1() : ads[channel / 2].readADC_Differential_2_3();
    } while ((readVal > 0 ? readVal : -readVal) < ranges[*gain_s] && *gain_s != 5);

    *result = readVal * voltageInputMultiplier[*gain_s][channel];
}

void readWithGain(int channel, float *result, int gain_s, int16_t *raw)
{
    ads[channel / 2].setGain(gains[gain_s]);
    *raw = channel % 2 == 0 ? ads[channel / 2].readADC_Differential_0_1() : ads[channel / 2].readADC_Differential_2_3();
    *result = *raw * voltageInputMultiplier[gain_s][channel];
}

void startAnalogInputs()
{

    for (byte i = 0; i < 6; i++)
    {
        for (byte j = 0; j < 8; j++)
        {
            voltageInputMultiplier[i][j] = 1;
        }
    }

    adsStatus[0] = ads[0].begin();
    adsStatus[1] = ads[1].begin(0x49);
    adsStatus[2] = ads[2].begin(0x4A);
    adsStatus[3] = ads[3].begin(0x4B);
}

void LoadAnalogInputGains(int channel, int setting, float value)
{
    voltageInputMultiplier[setting][channel] = value;
}

String ReturnAnalogInputGains()
{
    String ret = "{\"data\":[";
    for (byte i = 0; i < 8; i++)
    {
        ret += "{\"channel\":" + String(i);
        for (byte j = 0; j < 6; j++)
        {
            ret += ",\"" + String(j) + "\":" + String(voltageInputMultiplier[j][i]);
        }
        ret += i == 7 ? "}" : "},";
    }
    ret += "]}";
    return ret;
}

String printStatusADS()
{
    String res = "{ \"hardware\": ads, ";
    Serial.print("\n ADS STATUS:\n\t");
    for (byte i = 0; i < 4; i++)
    {
        Serial.print("ads");
        Serial.print(i);
        Serial.print(adsStatus[i] ? ": Active\n\t" : ": Inactive\n\t");
        res += "\"" + String(i) + "\": ";
        res += adsStatus[i] ? "1" : "0";
        if (i != 3)
            res += ", ";
    }
    res += "}";
    Serial.print("\n");

    return res;
}