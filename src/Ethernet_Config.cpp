#include <Arduino.h>
#include "Ethernet_Config.h"
#include <SPI.h>
#include <Ethernet.h>

EthernetClient ethClient;
IPAddress ipAddress;

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
    ipAddress.fromString(ETHERNET_IP);

    Ethernet.init(ETHERNET_CS_PIN);
    resetEthernet(ETHERNET_RESET_PIN);

    Serial.println("Starting ETHERNET connection...");
    Serial.println(ipAddress);
    Ethernet.begin(mac, ipAddress);
    delay(200);

    Serial.print("Ethernet IP is: ");
    Serial.println(Ethernet.localIP());
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