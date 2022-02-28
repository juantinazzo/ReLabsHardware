#include <Arduino.h>
#include "Ethernet_Config.h"
#include <SPI.h>
#include <Ethernet.h>
#include <utilities/Logger.h>

static char sys[] = "Eth_Config";

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

void resetEthernet(const uint8_t resetPin)
{
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    delay(50);
    digitalWrite(resetPin, LOW);
    delay(50);
    digitalWrite(resetPin, HIGH);
    delay(50);
}

void connectToEthernet()
{
    delay(50);
    byte *mac = new byte[6];
    macToArray(ETHERNET_MAC, mac);
    IPAddress ipAddress, ipFail;
    ipAddress.fromString(ETHERNET_IP);
    ipFail.fromString("0.0.0.0");
    Ethernet.init(ETHERNET_CS_PIN);
    resetEthernet(ETHERNET_RESET_PIN);

    LOG("Connecting via Ethernet", Info, sys);
    LOG("Desired IP Address: " + ipAddress.toString(), Info, sys);
    Ethernet.begin(mac, ipAddress);
    delay(200);
    if (ipAddress.toString() != ipFail.toString())
        LOG("Ethernet IP is: " + Ethernet.localIP().toString(), Info, sys);
    else
        LOG("Failed to connect to Ethernet, got IP: " + Ethernet.localIP().toString(), Error, sys);
}

void macToArray(const char *str, byte *bytes)
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