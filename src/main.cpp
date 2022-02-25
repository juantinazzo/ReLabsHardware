#include <Arduino.h>
#include "board.h"
#include <Ethernet.h>
#include <aWOT.h>
#include "network/Ethernet_Config.h"
#include "hardware_libs/IO_expander.h"
#include "network/Server_Handlers.h"
#include "utilities/Logger.h"
#include "systemManager.h"

#ifdef USE_WIFI
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "network/Passwords.h"
#endif
#ifdef USE_BT
#include "BluetoothSerial.h"
#endif

static char sys[] = "main.cpp";

/*

    Si da error de compilacion mirar la nota en Ethernet_Config.cpp

*/

#ifdef USE_WIFI
WiFiServer server(80);
#endif
#ifdef USE_BT
BluetoothSerial SerialBT;
#endif

EthernetServer ethernetServer(80);
Application app;
systemManager sM;

bool expanderStatus[8];

void setGetsPosts()
{
    app.get("/", &indexCmd);
    app.post("/analogInputs/", &handleAnalogInputs);
    app.post("/configGains/", &handleConfigGains);
    app.get("/readGains/", &handleReadGains);
    app.get("/status/", &handleStatus);
    app.post("/analogOutputs/", &handleAnalogOutputs);
}

void setup()
{
    randomSeed(micros());
    delay(50);
    Logger::SetPriority(Info);
    pinMode(led, OUTPUT);
    digitalWrite(led, HIGH);

#ifdef USE_WIFI
    LOG("Connecting to %s ", Info, sys, ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        LOG_NOTAG(".", Info, sys);
    }
    ArduinoOTA.setHostname("ReLabsModule");
    ArduinoOTA.begin();
    LOG("WiFi connected", Info, sys);
    LOG("IP address: " + WiFi.localIP().toString(), Info, sys);
    return true;
#endif

    setGetsPosts();

#ifdef USE_WIFI
    server.begin();
#endif
#ifdef USE_BT
    LOG("Started BT: %d", Info, sys, SerialBT.begin("ReLabsModule"));
#endif

    startExpanders();
    connectToEthernet();

    sM.startVI(0);
    sM.startVO(0);
    sM.startVO(4);
    sM.startIO(0);
    sM.startSERVO(SPARE_IO0);
}

void loop()
{

#ifdef USE_WIFI
    ArduinoOTA.handle();
    server.handleClient();
    WiFiClient client = server.available();
#endif
    EthernetClient ethernetClient = ethernetServer.available();

    if (ethernetClient.connected())
    {
        app.process(&ethernetClient);
        delay(5);
        ethernetClient.stop();
    }

#ifdef USE_WIFI
    if (client.connected())
    {
        app.process(&client);
        delay(5);
        client.stop();
    }
#endif

#ifdef USE_BT
    if (SerialBT.available())
    {
        Serial.write(SerialBT.read());
    }
#endif
}
