#include "Card.h"

#include <Arduino.h>

Card::Card(String new_name)
{
    name = new_name;
}

Card::~Card()
{
}

String Card::getStatus(void) const
{
    return "";
}

void Card::start()
{
}