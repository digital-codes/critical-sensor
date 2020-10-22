// communication mode
#ifndef CELL_MODE_H
#define CELL_MODE_H

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <Wire.h> // All sensors use I2C comunication.
#include <stdint.h>
#include <stdio.h>

#include "czSensor.h"
#include "uCRC16Lib.h"

/* select from communication mode

1) CELL_MODE
  Sim 800L

2) LORA_MODE
  Simple Lora 868

3) WIFI_MODE
*/

#define CELL_MODE 1
#define LORA_MODE 2
#define WIFI_MODE 3

void commsSetup();
bool commsTx(uint8_t *msg, uint8_t msgLen); //  receives ptr and len, returns sent OK
int commsRx(uint8_t *msg, uint8_t maxlen); // returns length, receives ptr and maxLen
void commsEnd();

// ------------------------------------------------
#if (COMMS_MODE == CELL_MODE)
#include "cellMode.h"

#endif
// ------------------------------------------------

// ------------------------------------------------
#if (COMMS_MODE == LORA_MODE)
#include "loraMode.h"

#endif
// ------------------------------------------------

#endif // CELL_MODE_H
