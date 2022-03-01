#include <Arduino.h>
#include "EthernetModule.h"
#include <SPI.h>
#include <Ethernet.h>
#include <utilities/Logger.h>
#include "utilities/ConfigSaver.h"

static char sys[] = "EthModule";
extern ConfigSaver CS;
/*
    Si da error de compilacion la parte de red cambiar en
    .platformio\packages\framework-arduinoespressif32\cores\esp32\Server.h

    class Server: public Print
        {
        public:
            void begin(uint16_t port = 0) {};
            void begin() {};
        };


*/

EthernetModule::EthernetModule()
{
}

void EthernetModule::reset()
{
    pinMode(ETH_RST, OUTPUT);
    digitalWrite(ETH_RST, HIGH);
    delay(50);
    digitalWrite(ETH_RST, LOW);
    delay(50);
    digitalWrite(ETH_RST, HIGH);
    delay(50);
}

void EthernetModule::connect()
{
    byte *mac = new byte[6];
    char macA[18], ipA[16];
    CS.getEth(ipA, macA);
    macToArray(macA, mac);
    IPAddress ipAddress;
    ipAddress.fromString(ipA);
    Ethernet.init(ETH_CS);
    reset();

    LOG("Connecting via Ethernet", Info, sys);
    LOG("Desired IP Address: " + ipAddress.toString(), Info, sys);
    Ethernet.begin(mac, ipAddress);
    delay(200);
    if (Ethernet.linkStatus() == LinkON)
        LOG("Ethernet IP is: " + Ethernet.localIP().toString(), Info, sys);
    else
    {
        LOG("Ethernet IP is: " + Ethernet.localIP().toString(), Info, sys);
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            LOG("No Ethernet module detected", Error, sys);
        }
        else if (Ethernet.linkStatus() == LinkOFF)
        {
            LOG("Ethernet cable not connected", Error, sys);
        }
        else
            LOG("Failed to connect to Ethernet, got IP: " + Ethernet.localIP().toString(), Error, sys);
    }
    extern IPAddress currentIP;
    currentIP = Ethernet.localIP();
}

void EthernetModule::macToArray(const char *str, byte *bytes)
{
    for (int i = 0; i < 6; i++)
    {
        bytes[i] = strtoul(str, NULL, 16);
        str = strchr(str, ':');
        if (str == NULL || *str == '\0')
        {
            break;
        }
        str++;
    }
}