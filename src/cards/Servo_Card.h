#ifndef SERVO_CARD_H
#define SERVO_CARD_H

#include <Arduino.h>
#include "Card.h"
#include "board.h"
#include <ESP32Servo.h>

class Servo_Card : public Card
{
public:
    Servo_Card();
    Servo_Card(uint8_t output_pin);
    void start();
    void setValue(int value);
    bool isRunning();
    void setOutputPin(uint8_t output_pin);
    void setMinUs(uint8_t min);
    void setMaxUs(uint8_t max);
    void stop();

private:
    uint8_t OUTPUT_p;
    uint16_t minUs = 1000;
    uint16_t maxUs = 2000;
    bool status;
    const char sys[11] = "Servo_Card";
    bool is_running = 0;
};
#endif