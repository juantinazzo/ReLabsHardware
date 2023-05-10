#include <Arduino.h>
#include "board.h"
#include <Ethernet.h>
#include <aWOT.h>
#include "network/EthernetModule.h"
#include "network/Server_Handlers.h"
#include "utilities/Logger.h"
#include "utilities/ConfigSaver.h"
#include "systemManager.h"
#include <ArduinoOTA.h>

#ifdef USE_WIFI
#include <WiFi.h>
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
IPAddress currentIP;
Application app;
systemManager sM;
ConfigSaver CS;
EthernetModule EM;

bool ethOk = true;
unsigned long timePassed = 0;
bool expanderStatus[8];

void setGetsPosts()
{
    app.get("/", &indexCmd);
    app.post("/analogInputs/", &handleAnalogInputs);
    app.post("/configGains/", &handleConfigGains);
    app.get("/readGains/", &handleReadGains);
    app.get("/status/", &handleStatus);
    app.post("/digitalIO/", &handleExp);
    app.post("/analogOutputs/", &handleAnalogOutputs);
    app.post("/wificredentials/", &handleWifi);
    app.post("/pendulo/", &handlePendulum);
    app.get("/pendulo/", &handlePendulum2);
}

void setup()
{
    randomSeed(micros());
    delay(50);
    Logger::SetPriority(Info);
    CS.begin();
    // CS.destroyEverthing();
#ifdef USE_WIFI
    char ssid[30], password[30];
    CS.getWiFi(ssid, password);
    LOG("Connecting to %s ", Info, sys, ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        LOG_NOTAG(".", Info, sys);
    }
    // ArduinoOTA.setHostname("ReLabsModule");
    // ArduinoOTA.begin();
    LOG("WiFi connected", Info, sys);
    LOG("IP address: " + WiFi.localIP().toString(), Info, sys);
#endif

    setGetsPosts();

#ifdef USE_WIFI
    server.begin();
#endif
#ifdef USE_BT
    LOG("Started BT: %d", Info, sys, SerialBT.begin("ReLabsModule"));
#endif
    char user[30], uploadpassword[30];
    CS.getOTA(user, uploadpassword);
    EM.connect();
    ArduinoOTA.begin(currentIP, user, uploadpassword, InternalStorage);
    sM.startRails();
    sM.setRails(true);
    // sM.startVI(0);
    // sM.startVO(0);
    //  sM.startVO(4);
    // sM.startIO(0);
    // sM.startSERVO(SLOT5_CS0);
    sM.startEXP(0);
}

void loop()
{

#ifdef USE_WIFI
    ArduinoOTA.handle();
    WiFiClient client = server.available();
#endif
    if (ethOk)
    {
        EthernetClient ethernetClient = ethernetServer.available();

        if (ethernetClient.connected())
        {
            app.process(&ethernetClient);
            delay(5);
            ethernetClient.stop();
        }
        ArduinoOTA.poll();
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

    if (timePassed + 500 < millis())
    {
        // Do domething
        timePassed = millis();
    }
}
