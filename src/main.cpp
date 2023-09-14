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
#include "network/Passwords.h"
#include "state_machines.h"
#include "Simple_MPU6050.h"

#define OFFSETS  1474,   232,   1004,   4294967221,   41,   8

//Adafruit_MPU6050 mpu;


#define MPU6050_DEFAULT_ADDRESS     0x68    // address pin low (GND), default for InvenSense evaluation board
Simple_MPU6050 mpu2;
const char DEVICE_NAME[] = "mpu6050";

unsigned long _time[2000][2]={}, _start_time=0;
float _angle[2000][2]={};
int index_use_array = 0;
int index_array_pos=0;


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

EthernetServer ethernetServer(8080);
IPAddress currentIP;
Application app;
systemManager sM;
ConfigSaver CS;
EthernetModule EM;
// Preferences preferences;

bool expanderStatus[8];

#define A_R 16384.0 // 32768/2
#define G_R 131.0 // 32768/250
 
//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779
float Acc[2];
float roll;
float pitch;
float combinedAngle;
float Gy[3];
float Angle[3];
long tiempo_prev;
float dt;
String valores;

#define FILTER_CONSTANT 0.8

// See mpu.on_FIFO(print_Values); in the Setup Loop
void print_Values (int16_t *gyro, int16_t *accel, int32_t *quat) {
  Quaternion q;
  VectorFloat gravity;
  float ypr[3] = { 0, 0, 0 };
  float xyz[3] = { 0, 0, 0 };
  mpu2.GetQuaternion(&q, quat);
  mpu2.GetGravity(&gravity, &q);
  mpu2.GetYawPitchRoll(ypr, &q, &gravity);
  mpu2.ConvertToDegrees(ypr, xyz);

  combinedAngle=acos(cos(xyz[2]/RAD_TO_DEG)*cos(xyz[1]/RAD_TO_DEG))*RAD_TO_DEG;

  index_array_pos++;
  if(index_array_pos>=2000){
    index_array_pos=0;
    index_use_array=index_use_array==0?1:0;
  }
  _time[index_array_pos][index_use_array]=millis()-_start_time;
  _angle[index_array_pos][index_use_array]=xyz[1];//Angle[0];
 /* Serial.print(xyz[0]);
  Serial.print(", ");
  Serial.print(xyz[1]);
  Serial.print(", ");
  Serial.println(xyz[2]);*/
   //Serial.printfloatx(F("Yaw")  , xyz[0],   9, 4, F(",   ")); //printfloatx is a Helper Macro that works with Serial.print that I created (See #define above)
    //Serial.printfloatx(F("Pitch"), xyz[1],   9, 4, F(",   "));
    //Serial.printfloatx(F("Roll") , xyz[2],   9, 4, F("\n"));
   /* Serial.printfloatx(F("ax")   , accel[0], 5, 0, F(",   "));
    Serial.printfloatx(F("ay")   , accel[1], 5, 0, F(",   "));
    Serial.printfloatx(F("az")   , accel[2], 5, 0, F(",   "));
    Serial.printfloatx(F("gx")   , gyro[0],  5, 0, F(",   "));
    Serial.printfloatx(F("gy")   , gyro[1],  5, 0, F(",   "));
    Serial.printfloatx(F("gz")   , gyro[2],  5, 0, F("\n"));*/
}

void mpu_setup()
{
  
  //mpu2.begin();
  mpu2.begin(SDA, SCL);
  // Setup the MPU
  mpu2.Set_DMP_Output_Rate_Hz(200);           // Set the DMP output rate from 200Hz to 5 Minutes.
  //mpu.Set_DMP_Output_Rate_Seconds(10);   // Set the DMP output rate in Seconds
  //mpu.Set_DMP_Output_Rate_Minutes(5);    // Set the DMP output rate in Minutes
  mpu2.SetAddress(MPU6050_DEFAULT_ADDRESS); //Sets the address of the MPU.
  //mpu2.CalibrateMPU();                      // Calibrates the MPU.
  mpu2.load_DMP_Image(OFFSETS);                    // Loads the DMP image into the MPU and finish configuration.
  mpu2.on_FIFO(print_Values);               // Set callback function that is triggered when FIFO Data is retrieved
  // Note that these funcitons return pointers to themselves so you can write them in one line
  // mpu.Set_DMP_Output_Rate_Hz(4).SetAddress(MPU6050_DEFAULT_ADDRESS).CalibrateMPU().load_DMP_Image().on_FIFO(print_Values);
  // Setup is complete!
}

void mpuTask(void *){
    while(true){
        //updateMPU();
        delay(5);
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
    app.post("/pendulo/", &handlePendulum);
    app.get("/pendulo/", &handlePendulum2);
    app.get("/pendulolisto/", &handlePendulum3);
}

void test_motor_task(void *pvParam){
  while(true){
  Serial.print(Angle[0]);
  Serial.print(",");
  Serial.print(Angle[1]);
  Serial.print(",");
  Serial.println(combinedAngle);
  delay(10);
  }
  vTaskDelete(NULL);
}

void setup()
{

    randomSeed(micros());
    delay(50);
    Logger::SetPriority(Info);

#ifdef USE_WIFI

    LOG("Connecting to %s ", Info, sys, ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        LOG_NOTAG(".", Info, sys);
    }
    LOG("WiFi connected", Info, sys);
    LOG("IP address: " + WiFi.localIP().toString(), Info, sys);
#endif

    setGetsPosts();

#ifdef USE_WIFI
    server.begin();
#endif
    char user[30], uploadpassword[30];
    sM.startRails();
    sM.setRails(true);
    mpu_setup();
    xTaskCreatePinnedToCore(pendulo_task,"pendulo_task", 8192,NULL,2,NULL,1);
 
}

void loop()
{
  mpu2.dmp_read_fifo(false);
  delay(5);

#ifdef USE_WIFI
    WiFiClient client = server.available();
#endif
    EthernetClient ethernetClient = ethernetServer.available();

    if (ethernetClient.connected())
    {
        app.process(&ethernetClient);
        delay(1);
        ethernetClient.stop();
    }
    ArduinoOTA.poll();
#ifdef USE_WIFI
    if (client.connected())
    {
        app.process(&client);
        delay(1);
        client.stop();
    }
#endif

}
