// critical zones sensor stuff
#ifndef CZSENSOR_H
#define CZSENSOR_H

#include <stdint.h>

extern const uint8_t deviceId;
extern const uint8_t gatewayId;
extern const char* deviceKey;
extern const uint8_t deviceFmt;
extern const uint8_t showLogo;
extern const uint8_t wifiMode;

// include devicespecs
#include "devicespec.h"

// define LORA gateway
#define GATEWAY_ID 0x8f
#define DEVICE_KEY "Critical Zones20"

#define LOOPS_PER_DAY 25 // 25

#ifndef DEVICE_FMT
#warning "NO FMT"
// initial format
typedef struct {
  uint8_t dummy[16];
  uint8_t id;
  uint8_t req;
  uint8_t bat;
  uint8_t rfu; // yet undefined
  uint16_t cnt;
  uint16_t light;
  uint16_t temp;
  uint16_t hum;
  uint16_t co2;
  uint16_t crc;
}  SensorMsg;

#else
#if (DEVICE_FMT == 1)
  typedef struct {
    uint8_t dummy[16];
    uint8_t id;
    uint8_t req;
    uint8_t bat;
    uint8_t light;
    uint16_t cnt;
    uint16_t temp;
    uint16_t hum;
    uint16_t co2;
    uint16_t prs;
    uint16_t crc;
  }  SensorMsg;
#else
  #error "Invalid FMT"
#endif

#endif

typedef enum {SensTypeData = 0x00, SensTypeCmd = 0x10, SensTypeTest = 0xf0} SensorReq;

//#define CZ_VERSION 2  // 2020-08-13

#define DBG


#endif // CZSENSOR_H
