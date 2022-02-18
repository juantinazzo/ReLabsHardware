#include <Arduino.h>
#include "Server_Handlers.h"
#include <ArduinoJson.h>
#include "Analog_Inputs.h"
#include "IO_expander.h"
#include "Voltage_Outputs.h"

DynamicJsonDocument doc(256);

void handleRoot()
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
}
