#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <ArduinoJson.h>
//#include <Ethernet.h>
#include "Passwords.h"
//#include "Ethernet_Config.h"
//#include <EthernetWebServer.h>

#define SDEF(sname, ...) S sname __VA_OPT__(= {__VA_ARGS__})

WebServer server(80);
// EthernetWebServer ethernetServer(80);

void connectToStuff();
void initOTA();
void reconnect();

// WiFiClient espClient;

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
    server.on("/", handleRoot);

    server.on("/analogInputs/", handleAnalogInputs);
    server.on("/configGains/", handleConfigGains);
    server.on("/readGains/", handleReadGains);
    server.on("/status/", handleStatus);
    server.on("/analogOutputs/", handleAnalogOutputs);

    server.onNotFound(handleNotFound);
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
    // connectToEthernet();
}

void loop()
{
    server.handleClient();
    ArduinoOTA.handle();
}

void connectToStuff()
{

    WiFi.disconnect();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void initOTA()
{
    ArduinoOTA.setHostname("ReLabs Module");
    ArduinoOTA.begin();
}
