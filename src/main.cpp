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
#include <Preferences.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;


#ifdef USE_WIFI
#include <WiFi.h>

//#include "network/Passwords.h"
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
// Preferences preferences;

bool expanderStatus[8];

void initMPU(){
     Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

}

void printMPU(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.print(" m/s^2\t");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.print(" rad/s\t");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

}

void mpuTask(void *){
    while(true){
        printMPU();
        delay(50);
    }
    vTaskDelete(NULL);
}

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
    CS.begin();

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
    char user[30], uploadpassword[30];
    CS.getOTA(user, uploadpassword);
    EM.connect();
    ArduinoOTA.begin(currentIP, user, uploadpassword, InternalStorage);
    sM.startRails();
    sM.setRails(true);
    //sM.startVI(0);
    //sM.startVO(0);
    //sM.startVO(4);
    sM.startIO(0);
    sM.startSERVO(SPARE_IO0);
    sM.startEXP(0);
    initMPU();
    xTaskCreatePinnedToCore(mpuTask,"mpu",2048,NULL,3,NULL,1);
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
    ArduinoOTA.poll();
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
