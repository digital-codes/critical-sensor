// wifi mode

#include <M5Stack.h>
#include "comms.h"
#include "czSensor.h"

#if (COMMS_MODE == WIFI_MODE)

#include "wifiMode.h"

#ifdef WIFIMULT
WiFiMulti wifiMulti;
#endif

extern uint16_t loopCnt;

void commsSetup() {
  Serial.println("Setup WiFI");
    Serial.println();

    int retries = 50;

#ifdef WIFIMULT
    for (int i=0;i<NUM_APS;i++){
        Serial.print("Adding SSID: ");
        Serial.println(wifi_ssid[i]);
        wifiMulti.addAP(wifi_ssid[i],wifi_pwd[i]);
    }

    if(wifiMulti.run(wifiApTimeoutMs) != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (0 == --retries) {
            Serial.println("Connect to AP failed");
            return;
        }
    }
#else
    Serial.print("Connecting to ");
    Serial.println(ssid);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(10, 200);
    M5.Lcd.printf("Trying: %s",ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (0 == --retries) {
            Serial.println("Connect to AP failed");
            return;
        }
    }
#endif

    Serial.println("");
    Serial.print("WiFi connected to");
    Serial.print(WiFi.SSID());
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // clear old text
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setCursor(10, 200);
    M5.Lcd.printf("Trying: %s",ssid);
    // set new text
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(10, 200);
    M5.Lcd.printf("%s connected",ssid);
    delay(2000);

}

bool commsTx(uint8_t *msg, uint8_t msgLen) { // len should be 32
  Serial.println("Send WiFi");

    // Use WiFiClient class to create TCP connections
#ifdef WIFIMULT
    if(wifiMulti.run(wifiApTimeoutMs) != WL_CONNECTED) {
        Serial.println("Not connected to AP");
        return 0;
    }
#else
#endif

    WiFiClient client;
    client.setTimeout(10 * 1000);

    if (!client.connect(host, port)) {
        Serial.println("connection failed");
        return 0;
    }

    // create message
    char hexmsg[2 * msgLen + 1] ;
    for (int i = 0; i < msgLen; i++) {
      sprintf(&hexmsg[2*i], "%02x", (int)msg[i]);
    }
    hexmsg[2*msgLen] = 0;

    Serial.print("Sending message: ");
    Serial.println(hexmsg);

    // This will send the request to the server
    client.print(String(hexmsg));

    Serial.println();
    Serial.println("closing connection");

    client.flush();
    delay(100);
    client.stop();
    while (client.connected() )
      Serial.println("Wait for disconnect");

  return 1;
}

int commsRx(uint8_t *msg, uint8_t maxlen) {
  Serial.println("Receive WiFi");
  return 0;
}

void commsEnd() {
  Serial.println("End WiFi");
  WiFi.disconnect();
  while (WiFi.status() == WL_CONNECTED) {
      delay(500);
  }
}

#endif
