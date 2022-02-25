#ifndef SERVER_HANDLERS_H
#define SERVER_HANDLERS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <aWOT.h>

#define DEF_HANDLER(name) void name(Request &req, Response &res)

DEF_HANDLER(indexCmd);
DEF_HANDLER(handleAnalogInputs);
DEF_HANDLER(handleAnalogOutputs);
DEF_HANDLER(handleConfigGains);
DEF_HANDLER(handleReadGains);
DEF_HANDLER(handleStatus);

#endif
