#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
extern "C"
{
#include "user_interface.h"
}
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

void connectToStuff();
void initOTA();
void reconnect();

WiFiClient espClient;

#include "Passwords.h"

float voltageInputMultiplier[6][8];
float voltageOutputMultiplier[4] = {1, 1, 1, 1};
u_int16_t voltageOutputOffset[4] = {2047, 2047, 2047, 2047};
bool adsStatus[4], expanderStatus[8], voltageOutputsStatus[4];

DynamicJsonDocument doc(256);

#include "Analog_Inputs.h"
#include "IO_expander.h"
#include "Voltage_Outputs.h"

void handleRoot()
{
    char temp[400];
    int sec = millis() / 1000;
    int min = sec / 60;
    int hr = min / 60;

    snprintf(temp, 400,

             "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

             hr, min % 60, sec % 60);
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
}

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

    initOTA();
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

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    setGetsPosts();
    server.begin();

    startAnalogInputs();
    startExpanders();
    startVoltageOutputs();
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
    ArduinoOTA.setHostname("ADQ");
    ArduinoOTA.begin();
}
