#ifndef ETH_RST
#include "board.h"
#endif

#ifndef ETHERNET_CONFIG_H
#define ETHERNET_CONFIG_H

#define ETHERNET_MAC "BA:E6:E3:B1:44:D8"
#define ETHERNET_IP "192.168.10.140"

#define ETHERNET_RESET_PIN ETH_RST
#define ETHERNET_CS_PIN ETH_CS

void resetEthernet(const uint8_t resetPin);

void connectToEthernet();
void macToArray(const char *str, byte *bytes);

#endif