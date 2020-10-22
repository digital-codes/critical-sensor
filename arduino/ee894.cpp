/*
        ee894 c02 sensor
*/
#include "sensors.h"

// implement only with MHZ19 sensor
#if (SENSOR_TYPE == SENS_EE894)

#include "ee894.h"
//#define I2C_DEBUG_TO_SERIAL

EE894::EE894(uint8_t id) {
  if (id == 0 || id > 126)
    _id = CO2_DEVICE_ADDR;
  else
    _id = id;
}

bool EE894::read(uint8_t address, uint8_t *data, uint8_t count) {
  uint8_t i = 0;
  if (Wire.requestFrom(address, (uint8_t)count)) {
    while (Wire.available()) {
      // Put read results in the Rx buffer
      data[i++] = Wire.read();
    }
    return true;
  }
  return false;
}


bool EE894::write(uint8_t address, uint8_t *data, uint8_t length) {
  bool function_result = false;

#ifdef I2C_DEBUG_TO_SERIAL
  Serial.printf("writeBytes:send to 0x%02x [0x%02x] data=", address);
#endif

  Wire.beginTransmission(address); // Initialize the Tx buffer
  for (int i = 0; i < length; i++) {
    Wire.write(*(data + i)); // Put data in Tx buffer
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("%02x ", *(data + i));
#endif
  }
  function_result = (Wire.endTransmission() == 0); // Send the Tx buffer

#ifdef I2C_DEBUG_TO_SERIAL
  Serial.printf("result:%s\n", function_result ? "OK" : "NG");
#endif

  return function_result; // Send the Tx buffer
}

uint8_t EE894::i2cCalcCRC8(uint8_t buf[], uint8_t from, uint8_t to) {
  uint8_t crcVal = CRC8_ONEWIRE_START;
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t curVal = 0;

  for (i = from; i < to; i++) {
    curVal = buf[i];
    for (j = 0; j < 8; j++) {
      if (((crcVal ^ curVal) & 0x80) != 0) // If MSBs are not equal
      {
        crcVal = ((crcVal << 1) ^ CRC8_ONEWIRE_POLY);
      } else {
        crcVal = (crcVal << 1);
      }
      curVal = curVal << 1;
    }
  }
  return crcVal;
}

float EE894::readTemperature() {
  // Reading command A: 0xE000
  // Read the temperature value in 0.01 Kelvin and relative humidity value in
  // 0.01 %. Write 2 bytes 0x66 => 0xe000 Read 6 bytes 0x67 => T-high, T-low,
  // CRC, H-high, H-low, CRC Temperature = (HI*256 + LO)/100 - 273.15 Use
  // -273.15 as error value
  _buf[0] = 0xe0;
  _buf[1] = 0x0;
  write(_id, _buf, 2);
  if (read(_id, _buf, 6))
    if (_buf[2] == i2cCalcCRC8(_buf, 0, 2))
      return (_buf[0] * 256 + _buf[1]) / 100. - 273.15;

  return -273.15;
}

float EE894::readHumidity() {
  // Reading command A: 0xE000
  // Read the temperature value in 0.01 Kelvin and relative humidity value in
  // 0.01 %. Write 2 bytes 0x66 => 0xe000 Read 6 bytes 0x67 => T-high, T-low,
  // CRC, H-high, H-low, CRC Humidity: (HI*256 + LO)/100 Use 0. as error value
  _buf[0] = 0xe0;
  _buf[1] = 0x0;
  write(_id, _buf, 2);
  if (read(_id, _buf, 6))
    if (_buf[5] == i2cCalcCRC8(_buf, 3, 5))
      return (_buf[3] * 256 + _buf[4]) / 100.;

  return 0.;
}

uint16_t EE894::readCo2() {
  // Reading command B: 0xE027
  // Read the averaged CO 2 value in 1 ppm, the raw CO 2 value in 1 ppm and
  // ambient pressure in 0.1 mbar
  // Write 2 bytes
  // 0x66 => 0xe027
  // Read 9 bytes
  // 0x67 => CO2-avg-high, CO2-avg-low, CRC, CO2-raw-high, CO2-raw-low, CRC,
  // P-high, P-low, CRC CO 2 average: (HI*256 + LO) CO 2 raw: (HI*256 + LO)
  // Pressure: (HI*256 + LO)/10 [mbar]
  // use filtered value after >= 10 samples
  // Use 0. as error value
  _buf[0] = 0xe0;
  _buf[1] = 0x27;
  write(_id, _buf, 2);
  if (read(_id, _buf, 9))
    if (_buf[2] == i2cCalcCRC8(_buf, 0, 2))
      return (_buf[0] * 256 + _buf[1]);

  return 0;
}

uint16_t EE894::readPressure() {
  // Reading command B: 0xE027
  // Read the averaged CO 2 value in 1 ppm, the raw CO 2 value in 1 ppm and
  // ambient pressure in 0.1 mbar
  // Write 2 bytes
  // 0x66 => 0xe027
  // Read 9 bytes
  // 0x67 => CO2-avg-high, CO2-avg-low, CRC, CO2-raw-high, CO2-raw-low, CRC,
  // P-high, P-low, CRC CO 2 average: (HI*256 + LO) CO 2 raw: (HI*256 + LO)
  // Pressure: (HI*256 + LO)/10 [mbar]
  // use filtered value after >= 10 samples
  // Use 0. as error value
  _buf[0] = 0xe0;
  _buf[1] = 0x27;
  write(_id, _buf, 2);
  if (read(_id, _buf, 9))
    if (_buf[8] == i2cCalcCRC8(_buf, 6, 8))
      return ((uint16_t)((_buf[6] * 256 + _buf[7])/10));

  return 0;
}
uint16_t EE894::readIntervall() {
  // Command for customer memory access: 0x7154
  // Command for measurement time configuration and customer adjustment
  // Write 3 (or more) bytes
  // 0x66 => 0x7154 + index + data-high + data-low + CRC
  // The CRC8 checksum for writing into the customer memory is built out of
  // the MEM ADR and the all DATA byte
  // for indices see user guide. important: index 0

  // index 0:
  //  measurement time interval, 2 byte in 1/10 seconds, range 15 – 3600 seconds
  // no crc on read here.
  _buf[0] = 0x71;
  _buf[1] = 0x54;
  _buf[2] = 0;
  write(_id, _buf, 3);
  delay(5); // maybe not needed here, but for SDC30 sensor
  read(_id, _buf, 2);
  return (uint16_t)((_buf[0] * 256 + _buf[1]) / 10.);
}

uint16_t EE894::writeIntervall(uint16_t sec) {
  // Command for customer memory access: 0x7154
  // Command for measurement time configuration and customer adjustment
  // Write 3 (or more) bytes
  // 0x66 => 0x7154 + index + data-high + data-low + CRC
  // The CRC8 checksum for writing into the customer memory is built out of
  // the MEM ADR and the all DATA byte
  // for indices see user guide. important: index 0

  // index 0:
  //  measurement time interval, 2 byte in 1/10 seconds, range 15 – 3600 seconds

  // ALways verify writes by reading values back!
  uint16_t ts = sec * 10; // sensor uses 10th of seconds
  _buf[0] = 0x71;
  _buf[1] = 0x54;
  _buf[2] = 0;
  _buf[3] = (uint8_t)(ts >> 8);   // 0x8c; // 0x150 = 336
  _buf[4] = (uint8_t)(ts & 0xff); //  0x80;
  _buf[5] = i2cCalcCRC8(_buf, 2, 5);
  write(_id, _buf, 6);

  // verify
  return readIntervall();
}

void EE894::printAll() {
	uint16_t co2, iv;
	float t,h,p;
	t = readTemperature();
  h = readHumidity();
  p = readPressure();
	co2 = readCo2();
	iv = readIntervall();
	Serial.printf("T: %f, H: %f, P: %f, CO2: %d, IV: %d\n",t,h,p,co2,iv);
}

#endif
