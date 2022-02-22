#include <Arduino.h>
#include "V_I_Card.h"
#include "utilities/Logger.h"

void V_I_Card::measureAndReturn(byte channel, String *text)
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
        // Serial.println(valueStr);
    }
    else
    {
        contentJson = "\"channel\":" + String(channel) + ", \"status\": \"error\"";
    }

    digitalWrite(led, HIGH);
    *text = contentJson;
}

void V_I_Card::measureAndReturnRAW(byte channel, String *text)
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

void V_I_Card::readWithRange(int channel, float *result, int *gain_s)
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

void V_I_Card::readWithGain(int channel, float *result, int gain_s, int16_t *raw)
{
    ads[channel / 2].setGain(gains[gain_s]);
    *raw = channel % 2 == 0 ? ads[channel / 2].readADC_Differential_0_1() : ads[channel / 2].readADC_Differential_2_3();
    *result = *raw * voltageInputMultiplier[gain_s][channel];
}

V_I_Card::V_I_Card(int addr_to_use) : Card()
{
    addr = addr_to_use;
    Start();
}

void V_I_Card::Start()
{

    for (byte i = 0; i < 6; i++)
    {
        for (byte j = 0; j < 4; j++)
        {
            voltageInputMultiplier[i][j] = 1;
        }
    }
    Wire.begin();
    if (addr == 0)
    {
        adsStatus[0] = ads[0].begin();
        adsStatus[1] = ads[1].begin(0x49);
    }
    else
    {
        adsStatus[0] = ads[0].begin(0x4A);
        adsStatus[1] = ads[1].begin(0x4B);
    }
    LOG("ADS%d Started", Info, sys, addr);
}

void V_I_Card::LoadGains(int channel, int setting, float value)
{
    voltageInputMultiplier[setting][channel] = value;
}

String V_I_Card::ReturnGains()
{
    String ret = "{\"data\":[";
    for (byte i = 0; i < 4; i++)
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

String V_I_Card::getStatus() const
{
    String res = "{ \"hardware\": ads, ";
    for (byte i = 0; i < 2; i++)
    {
        LOG("ads", Info, sys);
        LOG_NOTAG("%d", Info, sys, i);
        LOG_NOTAG(adsStatus[i] ? ": Active" : ": Inactive", Info, sys);
        res += "\"" + String(i) + "\": ";
        res += adsStatus[i] ? "1" : "0";
        if (i != 1)
            res += ", ";
    }
    res += "}";

    return res;
}

V_I_Card::~V_I_Card()
{
}