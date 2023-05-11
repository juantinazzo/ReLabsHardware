#include <Arduino.h>

#define ETH_CS 5
#define SDA 21
#define SCL 22
#define SCK 18
#define MISO 19
#define MOSI 23
#define POWER_RAILS 2

#define ETH_RST 15
#define TX0 1
#define RX0 3

#define SLOT0_CS0 4
#define SLOT0_CS1 16
#define SLOT1_CS0 17
#define SLOT1_CS1 13
#define SLOT2_CS0 12
#define SLOT3_CS0 14
#define SLOT4_CS0 27
#define SLOT5_CS0 26

#define SPARE_IO0 32
#define SPARE_IO1 33
#define SPARE_IO2 25

#define led 36 // 2

//#define USE_BT
#define USE_WIFI