#include <Arduino.h>
#include "board.h"
#ifdef USE_WIFI
#include <WiFi.h>
#include <ArduinoOTA.h>
#endif
#ifdef USE_BT
#include "BluetoothSerial.h"
#endif

#include <Ethernet.h>
#include <aWOT.h>
#include "network/Passwords.h"
#include "network/Ethernet_Config.h"
#include "hardware_libs/IO_expander.h"
//#include "cards/Voltage_Outputs.h"
#include "network/Server_Handlers.h"
#include "utilities/Logger.h"
#include "utilities/ioOffsetGains.h"

#include <ESP32Servo.h>

static char sys[] = "main.cpp";

/*

    Si da error de compilacion mirar la nota en Ethernet_Config.cpp

*/

#ifdef USE_WIFI
WiFiServer server(80);
#endif
#ifdef USE_BT
BluetoothSerial SerialBT;
#endif

EthernetServer ethernetServer(80);
Application app;

float voltageOutputMultiplier[4] = {1, 1, 1, 1};
uint16_t voltageOutputOffset[4] = {2047, 2047, 2047, 2047};
bool adsStatus[4], expanderStatus[8], voltageOutputsStatus[4];

#include <ESP32Servo.h>

// create four servo objects
Servo servo1;
ESP32PWM pwm;
// Published values for SG90 servos; adjust if needed
#define minUs 1000
#define maxUs 2000

#define servo1Pin 25
int pos = 0;

void setGetsPosts()
{
    app.get("/", &indexCmd);
    app.post("/analogInputs/", &handleAnalogInputs);
    app.post("/configGains/", &handleConfigGains);
    app.get("/readGains/", &handleReadGains);
    app.get("/status/", &handleStatus);
    app.post("/analogOutputs/", &handleAnalogOutputs);
}

void setup()
{
    randomSeed(micros());
    delay(50);
    Logger::SetPriority(Info);
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);

#ifdef USE_WIFI
    LOG("Connecting to %s ", Info, sys, ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        LOG_NOTAG(".", Info, sys);
    }
    ArduinoOTA.setHostname("ReLabsModule");
    ArduinoOTA.begin();
    LOG("WiFi connected", Info, sys);
    LOG("IP address: " + WiFi.localIP().toString(), Info, sys);
#endif

    setGetsPosts();

#ifdef USE_WIFI
    server.begin();
#endif

#ifdef USE_BT
    LOG("Started BT: %d", Info, sys, SerialBT.begin("ReLabsModule"));
#endif
    startExpanders();
    connectToEthernet();
    ESP32PWM::allocateTimer(0);
    servo1.setPeriodHertz(50); // Standard 50hz servo
    servo1.attach(servo1Pin, minUs, maxUs);
}

void loop()
{

#ifdef USE_WIFI
    ArduinoOTA.handle();
    server.handleClient();
    WiFiClient client = server.available();
#endif
    EthernetClient ethernetClient = ethernetServer.available();

    if (ethernetClient.connected())
    {
        app.process(&ethernetClient);
        delay(5);
        ethernetClient.stop();
        servo1.write(pos);
        pos += 5;
        if (pos > 180)
            pos = 0;
    }

#ifdef USE_WIFI
    if (client.connected())
    {
        app.process(&client);
        delay(5);
        client.stop();
    }
#endif

#ifdef USE_BT
    if (SerialBT.available())
    {
        Serial.write(SerialBT.read());
    }
#endif
}
