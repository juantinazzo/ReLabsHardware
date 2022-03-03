#include <Arduino.h>
#include "ConfigSaver.h"
#include <Preferences.h>
#include "Logger.h"
#include "ioOffsetGains.h"
#include "network/Passwords.h"
#include <nvs_flash.h>

Preferences preferences;

#define CPY(name, key) strcpy(name, preferences.getString(#key, "").c_str())
#define FOR4(var) for (uint8_t var; var < 4; var++)

static char sys[] = "ConfigSaver";
extern char temp[30];

ConfigSaver::ConfigSaver()
{
}

void ConfigSaver::destroyEverthing()
{
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init();  // initialize the NVS partition.
    //preferences.begin("hardware", false);
    //preferences.putBool("cleaned", true);
    //ESP.restart();
}

void ConfigSaver::begin()
{
   /* preferences.begin("hardware", false);
    if (preferences.isKey("cleaned") == 0)
    {
        destroyEverthing();
    }*/
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
    preferences.begin("hardware", false);
    if (preferences.isKey("MB_VER") == 0)
        preferences.putString("MB_VER", "V1.0.0");
    if (preferences.isKey("FIRM_VER") == 0)
        preferences.putString("FIRM_VER", "V0.7.1");
    char mb[10], sw[10];
    CPY(mb, MB_VER);
    CPY(sw, FIRM_VER);
    LOG("Motherboard version: %s\t Firmware version: %s", Info, sys, mb, sw);
    preferences.end();

    preferences.begin("wifi", false);
    if (preferences.isKey("AP") == 0)
    {
        preferences.putString("AP", ssid);
        preferences.putString("PWD", password);
        LOG("No WiFi config found. Setting up default from Passwords.h", Error, sys);
    }
    else
        LOG("WiFi config found", Info, sys);
    preferences.end();

    /*preferences.begin("gainOffset", true);
    if (preferences.isKey("OFFSETVO00") == 0)
    {
        preferences.end();
        ioOffsetGains iogainoffset;
        iogainoffset.setGain(1);
        iogainoffset.setOffset(0);
        FOR4(i)
        {
            setGainOffset(&iogainoffset, "VO",0,i);
        }
        FOR4(j)
        {
            setGainOffset(&iogainoffset, "VO", 1, j);
        }
    }
    preferences.end();*/
}

void ConfigSaver::setGainOffset(ioOffsetGains *data, char *name, uint8_t slot, uint8_t ch)
{
    preferences.begin("gainOffset", false);
    preferences.putFloat(joinName("OFFSET", name, slot, ch), data->getOffset());
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
    preferences.begin(name, true);
    data->setOffset(preferences.getFloat("OFFSET"));
    data->setGain(preferences.getFloat("GAIN"));
    preferences.end();
}

void ConfigSaver::getWiFi(char *accessPoint, char *password)
{
    preferences.begin("wifi", true);
    CPY(accessPoint, AP);
    CPY(password, PWD);
    preferences.end();
}

void ConfigSaver::getWiFi(char *accessPoint, char *password, char *ip)
{
    preferences.begin("wifi", true);
    CPY(accessPoint, AP);
    CPY(password, PWD);
    CPY(ip, IP);
    preferences.end();
}

void ConfigSaver::getEth(char *ip)
{
    preferences.begin("eth", true);
    CPY(ip, IP);
    preferences.end();
}

void ConfigSaver::getEth(char *ip, char *mac)
{
    preferences.begin("eth", true);
    CPY(ip, IP);
    CPY(mac, MAC);
    preferences.end();
}

void ConfigSaver::getOTA(char *username, char *password)
{
    preferences.begin("ota", true);
    CPY(username, USER);
    CPY(password, PWD);
    preferences.end();
}

void ConfigSaver::setHardware(char *item, char *value)
{
    preferences.begin("hardware", false);
    preferences.putString(item, value);
    preferences.end();
}

void ConfigSaver::getHardware(char *item, char *value)
{
    preferences.begin("hardware", true);
    CPY(value, item);
    preferences.end();
}

char *ConfigSaver::joinName(char *base, uint8_t slot, uint8_t channel)
{
    sprintf(temp, "%s%d%d", base, slot, channel);
    return temp;
}

char *ConfigSaver::joinName(char *base, char *secondbase, uint8_t slot, uint8_t channel)
{
    sprintf(temp, "%s%s%d%d", base, secondbase, slot, channel);
    return temp;
}