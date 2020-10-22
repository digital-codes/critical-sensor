/*
    Description: Read the analog quantity and digital quantity input by LIGHT
   Unit, and convert the analog quantity into 12-bit data for display
*/

#include "cz-logo.h"
#define M5STACK_MPU6886
#include <M5Stack.h>
#include <WiFi.h>
#include <stdint.h>
#include <stdio.h>
#include "czSensor.h"
#include "comms.h"

// sensors. see sensor.h for options
#include "sensors.h"

#include "mbedtls/aes.h"

#include "devicespec.h"
const uint8_t deviceId = DEVICE_ID; // set for each sensor
const uint8_t gatewayId = GATEWAY_ID;
const char* deviceKey = DEVICE_KEY;  // 16 bytes
#ifndef DEVICE_FMT
  #pragma message "Device format not set"
  const uint8_t deviceFmt = 0;   // default device format
#else
  #pragma message "Device format set"
  const uint8_t deviceFmt = DEVICE_FMT;
#endif
const uint8_t showLogo = SHOW_LOGO;
const uint8_t wifiMode = (COMMS_MODE == WIFI_MODE);


// rtc_data_areas survived deep sleep
RTC_DATA_ATTR uint16_t loopCnt = 0;

const static int loopDelay = (int)(24.0/(float)LOOPS_PER_DAY*3600.0); // almost 1 hour

void putLogo() {
  const uint16_t *pxd = (const uint16_t *)logo.pixel_data;
  const int lw = logo.width;
  const int lh = logo.height;

  int xoffs = (320 - lw) / 2;
  int yoffs = (240 - lh) / 2;

  int px = 0;
  for (int y = 0; y < lh; y++) {
    for (int x = 0; x < lw; x++) {
      M5.Lcd.drawPixel(xoffs + x, yoffs + y, pxd[px++]);
    }
  }

  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("ID %d, SNS %d, COM %d",deviceId,SENSOR_TYPE,COMMS_MODE);
}

void encrypt(char *plainText, int len, unsigned char *outputBuffer) {

  mbedtls_aes_context aes;

  uint8_t cbcIv[16]; // create random IV
  for (int i = 0; i < sizeof(cbcIv); i++) {
    cbcIv[i] = (uint8_t)random(48, 92); // provide lower + upper limit
  }

  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, (const unsigned char *)deviceKey,
                         strlen(deviceKey) * 8);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, len, cbcIv,
                        (const unsigned char *)plainText, outputBuffer);
  mbedtls_aes_free(&aes);
}

void hexDump(uint8_t* data, int len){
  for (int i=0;i<len;i++) {
    Serial.printf("%2.2x",data[i]);
  }
  Serial.println();
}

void setup() {

  M5.begin(true, false, false, false);

  Serial.begin(115200);
  Serial.println("Starting");

  // if btn a is pressed on reset, wait for release
  M5.update();
  if (showLogo){
    putLogo();
  } else {
	  if (M5.BtnA.read()){
		putLogo();
		while(1) {
		  delay(50);
		  M5.update();
		  if(!M5.BtnA.read()) break;
		}
	  }
  }

  // power down stuff
  powerConfig();

  commsSetup();

  randomSeed(deviceId); // seed with ID. for more randomness use readAnalog or so

#ifdef DBG
  Serial.printf("Msg size: %d\r\n", sizeof(SensorMsg));
#endif

  Serial.printf("Loop: %d\r\n", loopCnt);

  SensorMsg sm = readSensors(); // read sensors into data structure
  hexDump((uint8_t*)&sm,sizeof(SensorMsg));

  uint8_t cryptMsg[32];
  encrypt((char *)&sm, sizeof(cryptMsg), cryptMsg);
  hexDump(cryptMsg,sizeof(cryptMsg));

  if (commsTx(cryptMsg,sizeof(cryptMsg))) { // len should be 32
    Serial.println("Transmit OK");
  } else {
    Serial.println("Transmit error");
  }

  /*
  uint8_t rxMsg[2]; // only payload: ack value (2 byte)
  if (!commsRx(rxMsg,sizeof(rxMsg)))

  // parse for a packet, and call onReceive with the result:
  String resp = commsRx();
  Serial.println(resp);
  */

  commsEnd();

  //powerSave();
  Serial.printf("Go to sleep ...\r\n");
  Serial.flush(); // without, sleep will start while sending

  loopCnt++;

  M5.Power.setWakeupButton(BUTTON_B_PIN);

  // M5.Power.lightSleep(SLEEP_SEC(loopDelay));
  M5.Power.deepSleep(SLEEP_SEC(loopDelay)); //
}

void loop() {}
