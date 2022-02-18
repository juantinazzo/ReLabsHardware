/****************************************************************************************************************************
  defines.h
  EthernetWebServer is a library for the Ethernet shields to run WebServer

  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Built by Khoi Hoang https://github.com/khoih-prog/EthernetWebServer
  Licensed under MIT license
 ***************************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#if !defined(ESP32)
#error This code is for ESP32 only
#endif

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_ 3

#if (defined(ESP32))
// For ESP32
#warning Use ESP32 architecture
#define ETHERNET_USE_ESP32
#define BOARD_TYPE "ESP32"

#define W5500_RST_PORT 26
#endif

#ifndef BOARD_NAME
#define BOARD_NAME BOARD_TYPE
#endif

#include <SPI.h>

//#define USE_ETHERNET_WRAPPER    true
#define USE_ETHERNET_WRAPPER false

// Use true  for ENC28J60 and UIPEthernet library (https://github.com/UIPEthernet/UIPEthernet)
// Use false for W5x00 and Ethernetx library      (https://www.arduino.cc/en/Reference/Ethernet)

#define USE_ETHERNET true

#define USE_ETHERNET true
#include "Ethernet.h"
#warning Using Ethernet lib
#define SHIELD_TYPE "W5x00 using Ethernet Library"

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC 20

byte mac[][NUMBER_OF_MAC] =
    {
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13},
        {0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14},
};

// Select the IP address according to your local network
IPAddress ip(192, 168, 10, 140);

// Google DNS Server IP
IPAddress myDns(8, 8, 8, 8);

#endif // defines_h