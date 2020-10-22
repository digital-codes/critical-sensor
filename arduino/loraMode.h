// lora interface
#ifndef LORA_MODE_H
#define LORA_MODE_H

// !!!!!!!!!! LoRa library has wrong pin mapping for RESET and IRQ !!!!
#define LORA_ACTUAL_SS_PIN    5
#define LORA_ACTUAL_RESET_PIN 26
#define LORA_ACTUAL_DIO0_PIN  36
// Correct here!

typedef struct {
  uint8_t destAddr;
  uint8_t srcAddr;
  uint8_t msgId;
  uint8_t msgLen;
} LoraHdr;


#endif // LORA_MODE_H
