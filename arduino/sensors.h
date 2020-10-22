// sensor definitions
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <Wire.h> // All sensors use I2C comunication.
#include <stdint.h>
#include <stdio.h>
#include "czSensor.h"

// select sensor option
/*
1) EE894
  ee894 provides co2, temperature, humidity
2) MHZ19B
  mhz provides co2
  dht12 (from env/env hat) provides temperature, humidity
3) SDC30
  sdc30 provides co2, temperature, humidity
*/

#define SENS_EE894 1
#define SENS_MHZ19 2
#define SENS_SCD30 3

// select sensor
#include "devicespec.h"

// light sensor is same for all
#define LIGHT_AN_IN 35
#define LIGHT_DIGITAL_IN 2 // different to wifi  node32s (34)

#define POWER_CHARGE_PIN 25 // on M5 this is audio pin. make sure to have amplifier power off

#define BATTERY_IS_CHARGING 0x80
#define LIGHT_MAX_VALUE 4096

SensorMsg readSensors();
void powerConfig();

// -----------------------------------------
#if (SENSOR_TYPE == SENS_EE894)
#include "ee894.h"


#endif // EE894
// -----------------------------------------

// -----------------------------------------
#if (SENSOR_TYPE == SENS_MHZ19)
#include "mhz19.h"
#include "DHT12.h"
// we don't use the other ENV sensors here, include anyway
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

DHT12 dht12; //Preset scale CELSIUS and ID 0x5c.
Adafruit_BMP280 bme;
HardwareSerial co2sensor = Serial1;


#endif // MHZ19
// -----------------------------------------

// -----------------------------------------
#if (SENSOR_TYPE == SENS_SCD30)
#include "scd30.h"
// do not use the sparkfun library directly. Constructor
// initializes sensor intervall!
//#include <SparkFun_SCD30_Arduino_Library.h>

#endif // SDC30
// -----------------------------------------

#endif // SENSORS_H
