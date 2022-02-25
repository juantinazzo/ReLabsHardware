#include <Arduino.h>
#include "Servo_Card.h"
#include "board.h"
#include "utilities/Logger.h"

Servo servo;

Servo_Card::Servo_Card()
{
}

void Servo_Card::setOutputPin(uint8_t output_pin)
{
    OUTPUT_p = output_pin;
}

Servo_Card::Servo_Card(uint8_t output_pin)
{
    OUTPUT_p = output_pin;
    start();
}

void Servo_Card::start()
{
    servo.setPeriodHertz(50);
    if (servo.attach(OUTPUT_p, minUs, maxUs))
    {
        LOG("Servo started at pin", Info, sys, OUTPUT_p);
        is_running = true;
    }
    else
        LOG("Servo startup failed at pin ", Error, sys, OUTPUT_p);
}

void Servo_Card::setValue(int value)
{
    servo.write(value);
}
void Servo_Card::setMinUs(uint8_t min)
{
    minUs = min;
    stop();
    start();
}
void Servo_Card::setMaxUs(uint8_t max)
{
    maxUs = max;
    stop();
    start();
}
void Servo_Card::stop()
{
    servo.detach();
    LOG("Servo dettached at pin ", Info, sys, OUTPUT_p);
    is_running = false;
}

bool Servo_Card::isRunning()
{
    return is_running;
}
