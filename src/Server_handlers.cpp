#include <Arduino.h>
#include "Server_Handlers.h"
#include <ArduinoJson.h>
#include "Analog_Inputs.h"
#include "IO_expander.h"
#include "Voltage_Outputs.h"
#include <aWOT.h>

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
    res.printP(index);
}