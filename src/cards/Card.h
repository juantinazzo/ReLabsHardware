#ifndef CARD_H
#define CARD_H

#include <Arduino.h>

class Card
{
public:
    Card(String new_name = "null");
    virtual String getStatus(void) const;
    virtual void start();
    virtual ~Card();
    virtual bool isRunning();

private:
    String name;
    bool is_running = 0;
};

#endif