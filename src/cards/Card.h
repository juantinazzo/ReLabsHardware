#ifndef CARD_H
#define CARD_H

#include <Arduino.h>

class Card
{
public:
    Card(String new_name = "null");
    virtual String getStatus(void) const;
    virtual void Start();
    virtual ~Card();

private:
    String name;
};

#endif