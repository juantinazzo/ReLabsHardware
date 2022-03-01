#ifndef ETHERNET_MODULE_H
#define ETHERNET_MODULE_H

#include "board.h"
class EthernetModule
{
public:
    EthernetModule();
    void reset();
    void connect();

private:
    void macToArray(const char *str, byte *bytes);
};

#endif