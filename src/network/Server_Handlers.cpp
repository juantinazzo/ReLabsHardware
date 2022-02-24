#include <Arduino.h>
#include "Server_Handlers.h"
#include <ArduinoJson.h>
#include "hardware_libs/IO_expander.h"
//#include "cards/Voltage_Outputs.h"
#include <aWOT.h>
#include <ArduinoJson.h>
#include "systemManager.h"

extern systemManager sM;

DynamicJsonDocument doc(256);

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
            uint8_t val = value.toInt();
            uint8_t addr = val < 4 ? 0 : (val < 8 ? 1 : 2);
            if (argName == "ADCREAD")
            {
                sM.VI[addr].measureAndReturn(value.toInt(), &temp);
                message += "\t{ " + temp + " }";
                message += ",\n";
            }
            if (argName == "ADCRAW")
            {
                sM.VI[addr].measureAndReturnRAW(value.toInt(), &temp);
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
            uint8_t cn = (char)argName.substring(4).toInt();
            uint8_t index = cn < 4 ? 0 : 1;
            cn = index == 0 ? cn : cn - 4;
            if (argName.substring(0, 4) == "VSET")
            {
                sM.VO[index].setChannelVoltage((char)cn, value.toFloat());
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
                sM.VI[0].LoadGains(argName.toInt(), j, doc[String(j)]);
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
    res.print(sM.VI[0].ReturnGains());
    res.status(200);
}

void handleStatus(Request &req, Response &res)
{
    String status = "[{\"data\":";
    status += sM.VI[0].getStatus();
    status += ", " + printStatusExpander();
    status += "]}";
    res.set("Access-Control-Allow-Origin", "*");
    res.print(status);
    res.status(200);
}