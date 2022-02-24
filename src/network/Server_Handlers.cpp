#include <Arduino.h>
#include "Server_Handlers.h"
#include <ArduinoJson.h>
#include "hardware_libs/IO_expander.h"
//#include "cards/Voltage_Outputs.h"
#include <aWOT.h>
#include <ArduinoJson.h>
#include "cards/V_I_Card.h"
#include "cards/V_O_Card.h"

DynamicJsonDocument doc(256);
V_I_Card VI0(0);
V_O_Card VO0(0), VO4(4);

void indexCmd(Request &req, Response &res)
{

    // P macro for printing strings from program memory
    P(index) =
        "<html>\n"
        "<head>\n"
        "<title>Hello World!</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Greetings middle earth!</h1>\n"
        "</body>\n"
        "</html>";

    res.set("Content-Type", "text/html");
    res.set("Access-Control-Allow-Origin", "*");
    res.printP(index);
}

void handleAnalogInputs(Request &req, Response &res)
{

    String temp;
    char argNameC[10], valueC[2];
    String message = "{\"data\":[\n";
    while (req.left())
    {
        if (req.form(argNameC, 10, valueC, 10))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            if (argName == "ADCREAD")
            {
                if (value.toInt() < 4)
                    VI0.measureAndReturn(value.toInt(), &temp);
                message += "\t{ " + temp + " }";
                message += ",\n";
            }
            if (argName == "ADCRAW")
            {
                if (value.toInt() < 4)
                    VI0.measureAndReturnRAW(value.toInt(), &temp);
                message += "\t{ " + temp + " }";
                message += ",\n";
            }
        }
        else
        {
            return res.sendStatus(400);
        }
    }
    message = message.substring(0, message.length() - 2);
    message += "\n]}";
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print(message);
    res.status(200);
}

void handleAnalogOutputs(Request &req, Response &res)
{

    String message = "{\"data\":[\n";

    String temp;
    char argNameC[10], valueC[2];
    while (req.left())
    {
        if (req.form(argNameC, 10, valueC, 10))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            if (argName.substring(0, 4) == "VSET")
            {
                VO0.setChannelVoltage((char)argName.substring(4).toInt(), value.toFloat());
            }
        }
        else
        {
            return res.sendStatus(400);
        }
    }
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print("{\"status\":\"ok\"}");
    res.status(200);
}

void handleConfigGains(Request &req, Response &res)
{

    String temp;
    char argNameC[10], valueC[2];
    String message = "{\"data\":[\n";
    while (req.left())
    {
        if (req.form(argNameC, 10, valueC, 10))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            deserializeJson(doc, argName);
            for (uint8_t j = 0; j < 6; j++)
            {
                VI0.LoadGains(argName.toInt(), j, doc[String(j)]);
            }
        }
    }

    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print("{\"status\":\"ok\"}");
    res.status(200);
}

void handleReadGains(Request &req, Response &res)
{
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print(VI0.ReturnGains());
    res.status(200);
}

void handleStatus(Request &req, Response &res)
{
    String status = "[{\"data\":";
    status += VI0.getStatus();
    status += ", " + printStatusExpander();
    status += "]}";
    res.set("Access-Control-Allow-Origin", "*");
    res.print(status);
    res.status(200);
}