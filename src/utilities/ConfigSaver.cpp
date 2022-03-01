#include <Arduino.h>
#include "ConfigSaver.h"
#include <Preferences.h>
#include "Logger.h"
#include "ioOffsetGains.h"

Preferences preferences;

#define CPY(name, key) strcpy(name, preferences.getString(#key, "").c_str())

static char sys[] = "ConfigSaver";

ConfigSaver::ConfigSaver()
{
}

void ConfigSaver::begin()
{
    preferences.begin("ota", false);
    if (preferences.isKey("USER") == 0)
    {
        preferences.putString("USER", "ReLabsAdmin");
        preferences.putString("PWD", "RePassword");
        LOG("No OTA config found. Setting up default. Username: ReLabsAdmin, Password: RePassword", Error, sys);
    }
    else
        LOG("OTA config found", Info, sys);
    preferences.end();
    preferences.begin("eth", false);
    if (preferences.isKey("IP") == 0)
    {
        preferences.putString("IP", "192.168.10.140");
        preferences.putString("MAC", "BA:E6:E3:B1:44:D8");
        LOG("No Ethernet config found. Setting up default. IP: 192.168.10.140, MAC: BA:E6:E3:B1:44:D8", Error, sys);
    }
    else
        LOG("Ethernet config found", Info, sys);
    preferences.end();
}

void ConfigSaver::setGainOffset(ioOffsetGains *data, char *name)
{
    preferences.begin(name, false);
    preferences.putFloat("OFFSET", data->getOffset());
    preferences.putFloat("GAIN", data->getGain());
    preferences.end();
}

void ConfigSaver::setWiFi(char *accessPoint, char *password)
{
    preferences.begin("wifi", false);
    preferences.putString("AP", accessPoint);
    preferences.putString("PWD", password);
    preferences.end();
}

void ConfigSaver::setWiFi(char *accessPoint, char *password, char *ip)
{
    preferences.begin("wifi", false);
    preferences.putString("AP", accessPoint);
    preferences.putString("PWD", password);
    preferences.putString("IP", ip);
    preferences.end();
}

void ConfigSaver::setEth(char *ip)
{
    preferences.begin("eth", false);
    preferences.putString("IP", ip);
    preferences.end();
}

void ConfigSaver::setEth(char *ip, char *MAC)
{
    preferences.begin("eth", false);
    preferences.putString("IP", ip);
    preferences.putString("MAC", MAC);
    preferences.end();
}

void ConfigSaver::setOTA(char *username, char *password)
{
    preferences.begin("ota", false);
    preferences.putString("USER", username);
    preferences.putString("PWD", password);
    preferences.end();
}

void ConfigSaver::getGainOffset(ioOffsetGains *data, char *name)
{
    preferences.begin(name, false);
    data->setOffset(preferences.getFloat("OFFSET"));
    data->setGain(preferences.getFloat("GAIN"));
    preferences.end();
}

void ConfigSaver::getWiFi(char *accessPoint, char *password)
{
    preferences.begin("wifi", false);
    CPY(accessPoint, AP);
    CPY(password, PWD);
    preferences.end();
}

void ConfigSaver::getWiFi(char *accessPoint, char *password, char *ip)
{
    preferences.begin("wifi", false);
    CPY(accessPoint, AP);
    CPY(password, PWD);
    CPY(ip, IP);
    preferences.end();
}

void ConfigSaver::getEth(char *ip)
{
    preferences.begin("eth", false);
    CPY(ip, IP);
    preferences.end();
}

void ConfigSaver::getEth(char *ip, char *mac)
{
    preferences.begin("eth", false);
    CPY(ip, IP);
    CPY(mac, MAC);
    preferences.end();
}

void ConfigSaver::getOTA(char *username, char *password)
{
    preferences.begin("ota", false);
    CPY(username, USER);
    CPY(password, PWD);
    preferences.end();
}
