// lora mode

#include "comms.h"
#include "czSensor.h"
#include "loraMode.h"

#if (COMMS_MODE == LORA_MODE)

#include <M5LoRa.h>

extern uint16_t loopCnt;

static int onReceive(int packetSize, uint8_t *msg, uint8_t maxlen) {
  int incoming = 0;
  LoraHdr hdr = {0};
  if (packetSize == 0)
    return 0; // if there's no packet, return

  Serial.println("Lora Packet received");
  while (LoRa.available()) {
    uint8_t r = (uint8_t)LoRa.read();
    // check header: dev id (destination), src id (gateway), original cnt,
    // length (2)
    switch (incoming) {
    case 0:
      hdr.destAddr = r;
      break;
    case 1:
      hdr.srcAddr = r;
      break;
    case 2:
      hdr.msgId = r;
      break;
    case 3:
      hdr.msgLen = r;
      break;
    default:
      uint8_t m = incoming - sizeof(hdr);
      if (m < maxlen)
        msg[m] = r; // skip header, keep raw msg only
      break;
    }
    incoming++;
  }
  incoming -= sizeof(hdr); // subtract header len
  // check header, don't care about msgId here
  if ((hdr.destAddr != deviceId) || (hdr.srcAddr != gatewayId) ||
      (hdr.msgLen != incoming)) {
    Serial.println("LoRa hdr invalid");
    return 0;
  } else {
    Serial.println("LoRa packet OK");
    return hdr.msgLen;
  }
}

void commsSetup() {
  // start lora early
  Serial.println("Setup LoRa");
  LoRa.setPins(LORA_ACTUAL_SS_PIN, LORA_ACTUAL_RESET_PIN, LORA_ACTUAL_DIO0_PIN);
  LoRa.begin(868E6); // start LoRa at 868 MHz
  // wait 1 s
  delay(1000);
}

bool commsTx(uint8_t *msg, uint8_t msgLen) { // len should be 32
  // lora write is single byte, or char*,len
  LoraHdr hdr = {gatewayId, deviceId, (uint8_t)loopCnt, msgLen};
  uint16_t xcrc = uCRC16Lib::calculate((char *)&hdr, sizeof(hdr));
  uint16_t resp;
  bool rc = false;
  Serial.printf("Expected crc: %4.4x\n", xcrc);

  Serial.println("HDR:");
  for (int i=0;i<sizeof(hdr);i++){
    Serial.printf("%2.2x",((uint8_t*)&hdr)[i]);
  }
  Serial.println("");
  Serial.println("MSG:");
  for (int i=0;i<msgLen;i++){
    Serial.printf("%2.2x",msg[i]);
  }
  Serial.println("");

  Serial.println("Send LoRa");
  // send with max 10 retries
  for (int i = 0; i < 10; i++) {
    LoRa.beginPacket(); // start packet
    LoRa.write((uint8_t *)&hdr, sizeof(hdr)); // send hdr
    LoRa.write(msg, msgLen);                  // add payload
    LoRa.endPacket();                         // finish packet and send it
    // wait for ack
    int rxLen = commsRx((uint8_t *)&resp, sizeof(resp));
    if ((rxLen == sizeof(resp)) && (resp == xcrc)) {
      // done if ACK ok
      Serial.println("ACK OK");
      rc = true;
      break;
    }
    Serial.printf("Invalid ACK: %d =>  %4.4x (%4.4x)\n", rxLen, resp, xcrc);
    delay(random(500, 1500));
  }
  return rc;
}

int commsRx(uint8_t *msg, uint8_t maxlen) {
  Serial.println("Receive LoRa");
  // wait 1 s for response
  LoRa.receive();
  delay(100);
  // parse for a packet, and call onReceive with the result:
  int rx = onReceive(LoRa.parsePacket(), msg, maxlen);
  return rx;
}

void commsEnd() {
  Serial.println("End LoRa");
  LoRa.end();
}

#endif
