#ifndef SERVER_HANDLERS_H
#define SERVER_HANDLERS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <aWOT.h>

#define DEF_HANDLER(name) void name(Request &req, Response &res)
/*GENERIC HANDLERS*/
DEF_HANDLER(indexCmd);
DEF_HANDLER(handleAnalogInputs);
DEF_HANDLER(handleAnalogOutputs);
DEF_HANDLER(handleConfigGains);
DEF_HANDLER(handleReadGains);
DEF_HANDLER(handleStatus);
DEF_HANDLER(handleExp);
DEF_HANDLER(handleWifi);


/*APP SPECIFIC HANDLERS*/
DEF_HANDLER(handlePendulum);
DEF_HANDLER(handlePendulum2);

#endif
