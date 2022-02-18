#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <ArduinoJson.h>
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
DynamicJsonDocument doc(256);

void connectToStuff();
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

/*void handleRoot()
{
    char temp[400];
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/html", temp);
}

void handleAnalogInputs()
{
    if (server.method() != HTTP_POST)
    {
        digitalWrite(led, 1);
        server.send(405, "text/plain", "Method Not Allowed");
        digitalWrite(led, 0);
    }
    else
    {
        String temp;
        String message = "{\"data\":[\n";
        for (uint8_t i = 0; i < server.args(); i++)
        {
            if (server.argName(i) == "ADCREAD")
            {
                measureAndReturn(server.arg(i).toInt(), &temp);
                message += "\t{ " + temp + " }";
                if (i < server.args() - 1)
                    message += ",\n";
            }
            if (server.argName(i) == "ADCRAW")
            {
                measureAndReturnRAW(server.arg(i).toInt(), &temp);
                message += "\t{ " + temp + " }";
                if (i < server.args() - 1)
                    message += ",\n";
            }
        }
        message += "\n]}";
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", message);
    }
}

void handleAnalogOutputs()
{
    if (server.method() != HTTP_POST)
    {
        digitalWrite(led, 1);
        server.send(405, "text/plain", "Method Not Allowed");
        digitalWrite(led, 0);
    }
    else
    {
        String message = "{\"data\":[\n";
        for (uint8_t i = 0; i < server.args(); i++)
        {
            if (server.argName(i).substring(0, 4) == "VSET")
            {
                setChannelVoltage((char)server.argName(i).substring(4).toInt(), server.arg(i).toFloat());
            }
        }
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "text/plain", "{\"status\":\"ok\"}");
    }
}

void handleNotFound()
{
    digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(404, "text/plain", message);
    digitalWrite(led, 0);
}

void handleConfigGains()
{
    if (server.method() != HTTP_POST)
    {
        digitalWrite(led, 1);
        server.send(405, "text/plain", "Method Not Allowed");
        digitalWrite(led, 0);
    }
    else
    {

        for (uint8_t i = 0; i < server.args(); i++)
        {
            deserializeJson(doc, server.arg(i));
            for (uint8_t j = 0; j < 6; j++)
            {
                LoadAnalogInputGains(server.argName(i).toInt(), j, doc[String(j)]);
            }
        }
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Listo ;) ");
}

void handleReadGains()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", ReturnAnalogInputGains());
}

void handleStatus()
{
    String status = "[{\"data\":";
    status += printStatusADS();
    status += ", " + printStatusExpander();
    status += "]}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", status);
}*/

void setGetsPosts()
{
    /*  server.on("/", handleRoot);

      server.on("/analogInputs/", handleAnalogInputs);
      server.on("/configGains/", handleConfigGains);
      server.on("/readGains/", handleReadGains);
      server.on("/status/", handleStatus);
      server.on("/analogOutputs/", handleAnalogOutputs);

      server.onNotFound(handleNotFound);*/

    app.get("/", &indexCmd);
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
        ethernetClient.stop();
    }
    if (client.connected())
    {
        app.process(&client);
    }
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
