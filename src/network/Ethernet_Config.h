#ifndef ETH_RST
#include "board.h"
#endif

#define ETHERNET_MAC "BA:E6:E3:B1:44:D8" // Ethernet MAC address (have to be unique between devices in the same network)
#define ETHERNET_IP "192.168.10.140"     // IP address of RoomHub when on Ethernet connection

#define ETHERNET_RESET_PIN ETH_RST // ESP32 pin where reset pin from W5500 is connected
#define ETHERNET_CS_PIN ETH_CS     // ESP32 pin where CS pin from W5500 is connected

void resetEthernet(const uint8_t resetPin);

void connectToEthernet();
void macToArray(const char *str, byte *bytes);