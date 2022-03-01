#ifndef CONFIG_SAVER_H
#define CONFIG_SAVER_H

#include "ioOffsetGains.h"
#include <Arduino.h>

class ConfigSaver
{
public:
    ConfigSaver();
    void setGainOffset(ioOffsetGains *data, char *name);
    void setWiFi(char *accessPoint, char *password);
    void setWiFi(char *accessPoint, char *password, char *ip);
    void setEth(char *ip);
    void setEth(char *ip, char *MAC);
    void setOTA(char *username, char *password);
    void getGainOffset(ioOffsetGains *data, char *name);
    void getWiFi(char *accessPoint, char *password);
    void getWiFi(char *accessPoint, char *password, char *ip);
    void getEth(char *ip);
    void getEth(char *ip, char *MAC);
    void getOTA(char *username, char *password);
    void begin();

private:
};

#endif