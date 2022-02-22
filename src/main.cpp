#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>

#include <Ethernet.h>
#include "network/Passwords.h"
#include "network/Ethernet_Config.h"
#include <aWOT.h>

static char sys[] = "main.cpp";

/*

    Si da error de compilacion mirar la nota en Ethernet_Config.cpp

*/

WiFiServer server(80);
EthernetServer ethernetServer(80);
Application app;

void initOTA();
void reconnect();

float voltageInputMultiplier[6][8];
float voltageOutputMultiplier[4] = {1, 1, 1, 1};
uint16_t voltageOutputOffset[4] = {2047, 2047, 2047, 2047};
bool adsStatus[4], expanderStatus[8], voltageOutputsStatus[4];

#include "cards/Analog_Inputs.h"
#include "hardware_libs/IO_expander.h"
#include "cards/Voltage_Outputs.h"
#include "network/Server_Handlers.h"

#include "utilities/Logger.h"

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
    Wire.begin();
    randomSeed(micros());
    delay(50);
    Logger::SetPriority(Info);
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);
    LOG_NOTAG("\n\n", Info, sys);
    LOG("Connecting to %s", Info, sys, ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        LOG_NOTAG(".", Info, sys);
    }
    LOG_NOTAG("\n", Info, sys);
    initOTA();
    LOG("WiFi connected", Info, sys);
    LOG("IP address: " + WiFi.localIP().toString(), Info, sys);

    setGetsPosts();
    server.begin();

    startAnalogInputs();
    startExpanders();
    startVoltageOutputs();
    connectToEthernet();
}

void loop()
{
    // server.handleClient();
    ArduinoOTA.handle();
    WiFiClient client = server.available();
    EthernetClient ethernetClient = ethernetServer.available();
    if (ethernetClient.connected())
    {
        app.process(&ethernetClient);
        delay(5);
        ethernetClient.stop();
    }
    if (client.connected())
    {
        app.process(&client);
        delay(5);
        client.stop();
    }
}
void initOTA()
{
    ArduinoOTA.setHostname("ReLabsModule");
    ArduinoOTA.begin();
}
