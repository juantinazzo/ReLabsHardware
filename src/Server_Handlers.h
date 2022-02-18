#include <Arduino.h>
#include <ArduinoJson.h>
#include <aWOT.h>

void indexCmd(Request &req, Response &res);
void handleAnalogInputs(Request &req, Response &res);
void handleAnalogOutputs(Request &req, Response &res);
void handleConfigGains(Request &req, Response &res);
void handleReadGains(Request &req, Response &res);
void handleStatus(Request &req, Response &res);
