#include <Arduino.h>
#include <ArduinoJson.h>
#include <aWOT.h>

// extern EthernetWebServer ethernetServer;

void handleRoot();
void handleAnalogInputs();
void handleAnalogOutputs();
void handleNotFound();
void handleConfigGains();
void handleReadGains();
void handleStatus();

void indexCmd(Request &req, Response &res);