#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>

#include <Ethernet.h>
#include "Passwords.h"
#include "Ethernet_Config.h"
#include <aWOT.h>

/*

    Si da error de compilacion mirar la nota en Ethernet_Config.cpp

*/

#define SDEF(sname, ...) S sname __VA_OPT__(= {__VA_ARGS__})

WiFiServer server(80);
EthernetServer ethernetServer(80);
Application app;

void initOTA();
void reconnect();

float voltageInputMultiplier[6][8];
float voltageOutputMultiplier[4] = {1, 1, 1, 1};
uint16_t voltageOutputOffset[4] = {2047, 2047, 2047, 2047};
bool adsStatus[4], expanderStatus[8], voltageOutputsStatus[4];

#include "Analog_Inputs.h"
#include "IO_expander.h"
#include "Voltage_Outputs.h"
#include "Server_Handlers.h"

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
    Serial.begin(115200);
    Wire.begin();
    randomSeed(micros());
    delay(50);

    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    initOTA();
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

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
