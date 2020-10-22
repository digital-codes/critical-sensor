// cellular mode

#include "comms.h"
#include "cellMode.h"

#if (COMMS_MODE == CELL_MODE)

#define SIM_RX_PIN 16
#define SIM_TX_PIN 17
#define SIM_RESET_PIN 5

// globals
HardwareSerial radio = Serial2;

static String resp;

static int startRadio() {
  radio.print(F("AT+CPIN=?\r\n"));
  delay(50);
  radio.print(F("AT+IPR=115200\r\n"));
  delay(50);
  radio.print(F("AT+CMEE=2\r\n"));
  delay(50);

  return true;
}

static String readRadio(uint32_t timeout) {

  uint64_t timeOld = millis();

  while (!radio.available() && !(millis() > timeOld + timeout)) {
    delay(13);
  }

  String str;

  while (radio.available()) {
    if (radio.available() > 0) {
      str += (char)radio.read();
    }
  }

  return str;
}

String commsRx() { return readRadio(5000); }

void commsEnd() {}

void commsSetup() {
  // init and power up GSM
  Serial.printf("Radio init\n");
  resp.reserve(255);
  radio.begin(115200, SERIAL_8N1, SIM_RX_PIN, SIM_TX_PIN);
  pinMode(SIM_RESET_PIN, OUTPUT);
}

bool commsTx(uint8_t *cryptMsg, uint8_t cryptLen) {
  if (startRadio()) {
    Serial.printf("Radio\n");
    radio.print(F("AT\r\n"));
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);

    resp = "";
    while (resp.indexOf("1") == -1) {
      radio.print(F("AT+CGATT?\r\n"));
      resp = readRadio(5000);
      Serial.print(resp);
      delay(500);
    }

    radio.print(F("AT+CSTT=\"iot.1nce.net\",\"\",\"\"\r\n"));
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);

    radio.print(F("AT+CIICR\r\n"));
    delay(1000);
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);

    Serial.println("Wait for address");
    delay(200);

    radio.print(F("AT+CIFSR\r\n"));
    delay(200);
    resp = readRadio(5000);
    Serial.print(resp);

    delay(200);
    radio.print(F("at+cipstart=\"tcp\",\"84.38.68.154\",\"50351\"\r\n"));
    delay(500);
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);
    // should be CONNECT OK

    delay(200);
    radio.print(F("AT+CIPSEND\r\n"));
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);

    delay(200);
    radio.print(F("\""));
    for (int i = 0; i < cryptLen; i++) {
      char str[3];
      sprintf(str, "%02x", (int)(cryptMsg[i]));
      radio.print(str);
    }
    radio.print(F("\""));
    radio.printf("%c", 0x1A); // EOF
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);

    delay(200);
    radio.print(F("AT+CIPSHUT\r\n"));
    delay(10);
    resp = readRadio(5000);
    Serial.print(resp);
  }
  return true;
}

#endif
