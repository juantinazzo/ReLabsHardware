#include <Arduino.h>
#include "Server_Handlers.h"
#include <ArduinoJson.h>
#include <aWOT.h>
#include <ArduinoJson.h>
#include "systemManager.h"
#include "utilities/ConfigSaver.h"

extern systemManager sM;
extern ConfigSaver CS;

DynamicJsonDocument doc(256);


DEF_HANDLER(indexCmd)
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
    char pwd[20], user[20];
    CS.getOTA(user, pwd);
    res.printf("User: %s\n Password: %s", user, pwd);
}

DEF_HANDLER(handlePendulum)
{

    String temp;
    char argNameC[10], valueC[2];
    while (req.left())
    {
        if (req.form(argNameC, 10, valueC, 10))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            deserializeJson(doc, argName);

            if(argName=="init"){
                float angle=doc["Angle"];
                float lenght=doc["Lenght"];
                int massIndex=(int)doc["MassIndex"];
            }
        }

    }
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print("{\"status\":\"ok\"}");
    res.status(200);
}

DEF_HANDLER(handlePendulum2)
{

    String temp;
    char argNameC[10], valueC[2];
    while (req.left())
    {
        if (req.form(argNameC, 10, valueC, 10))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            deserializeJson(doc, argName);

            if(argName=="init"){
                float angle=doc["Angle"];
                float lenght=doc["Lenght"];
                int massIndex=(int)doc["MassIndex"];
            }
        }

    }

    String ret = "{\"data\":[";
    for (byte i = 0; i < 4; i++)
    {
        //ret += "{\""+dataTime(i)+"\":";
        for (byte j = 0; j < 6; j++)
        {
          //  ret += ",\"" + String(j) + "\":" + String(V_I_OG[j][i].getGain());
        }
        ret += i == 7 ? "}" : "},";
    }
    ret += "]}";

    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print("{\"status\":\"ok\"}");
    res.status(200);
}

DEF_HANDLER(handleAnalogInputs)
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

DEF_HANDLER(handleAnalogOutputs)
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


DEF_HANDLER(handleConfigGains)
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

DEF_HANDLER(handleReadGains)
{
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print(sM.VI[0].ReturnGains());
    res.status(200);
}

DEF_HANDLER(handleStatus)
{
    String status = "[{\"data\":";
    status += sM.VI[0].getStatus();
    // status += ", " + printStatusExpander();
    status += "]}";
    res.set("Access-Control-Allow-Origin", "*");
    res.print(status);
    res.status(200);
}

DEF_HANDLER(handleExp)
{
    String temp;
    char argNameC[20], valueC[10];
    String message = "{\"data\":[\n";
    while (req.left())
    {
        if (req.form(argNameC, 20, valueC, 10))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            uint16_t val = value.toInt();
            uint8_t cn = (char)argName.substring(7).toInt();
            if (argName.substring(0, 7) == "EXPREAD")
            {
                temp = String(sM.EXP[cn].readInputs());
                message += "\t{\"values\":" + temp + " }";
                message += ",\n";
            }
            if (argName.substring(0, 7) == "EXPWRTE")
            {
                sM.EXP[cn].setOutputs(val);
                message += "\t{\"status\":\"ok\"}";
                message += ",\n";
            }
            if (argName.substring(0, 7) == "EXPCONF")
            {
                sM.EXP[cn].configure(val);
                message += "\t{\"status\":\"ok\"}";
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

DEF_HANDLER(handleWifi)
{
    String temp;
    char argNameC[40], valueC[40];
    String ssid, password;
    String message = "{\"data\":[\n";
    while (req.left())
    {
        if (req.form(argNameC, 40, valueC,40))
        {
            String argName = String(argNameC);
            String value = String(valueC);
            if (argName == "SSID")
            {
                ssid = value;
                message += "\t{\"status\":\"ok\"}";
                message += ",\n";
            }
            if (argName == "PWD")
            {
                password = value;
                message += "\t{\"status\":\"ok\"}";
                message += ",\n";
            }
        }
        else
        {
            return res.sendStatus(400);
        }
    }
    CS.setWiFi((char *)ssid.c_str(), (char *)password.c_str());
    message = message.substring(0, message.length() - 2);
    message += "\n]}";
    res.set("Access-Control-Allow-Origin", "*");
    res.set("Content-Type", "text/plain");
    res.print(message);
    res.status(200);
}