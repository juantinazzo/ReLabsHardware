#include <Arduino.h>
#include <WebServer.h>
//#include <EthernetWebServer.h>
#include <ArduinoJson.h>

extern WebServer server;
// extern EthernetWebServer ethernetServer;

void handleRoot();
void handleAnalogInputs();
void handleAnalogOutputs();
void handleNotFound();
void handleConfigGains();
void handleReadGains();
void handleStatus();