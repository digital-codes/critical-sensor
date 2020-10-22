/*
	ee894 c02 sensor
*/

#ifndef EE894_h
#define EE894_h
#include "Arduino.h"
#include "Wire.h"



// I2C addressing
/*
The slave address is 0x33. Accordingly, the address byte 0x67 is used to read and 0x66 to write,
please refer to the NXP specs section 3.1.10.
The I2C interface of the EE894 module supports reading the measured data as well as writing in the
customer memory.
*/

#define CO2_DEVICE_ADDR 0x33


// List of supported commands:


// Reading command A: 0xE000
// Read the temperature value in 0.01 Kelvin and relative humidity value in 0.01 %.
// Write 2 bytes
// 0x66 => 0xe000
// Read 6 bytes
// 0x67 => T-high, T-low, CRC, H-high, H-low, CRC
// Temperature = (HI*256 + LO)/100 - 273.15
// Humidity: (HI*256 + LO)/100

// Reading command B: 0xE027
// Read the averaged CO 2 value in 1 ppm, the raw CO 2 value in 1 ppm and
// ambient pressure in 0.1 mbar
// Write 2 bytes
// 0x66 => 0xe027
// Read 9 bytes
// 0x67 => CO2-avg-high, CO2-avg-low, CRC, CO2-raw-high, CO2-raw-low, CRC, P-high, P-low, CRC
// CO 2 average: (HI*256 + LO)
// CO 2 raw: (HI*256 + LO)
// Pressure: (HI*256 + LO)/10 [mbar]

// Command for customer memory access: 0x7154
// Command for measurement time configuration and customer adjustment
// Write 3 (or more) bytes
// 0x66 => 0x7154 + index + data-high + data-low + CRC
// The CRC8 checksum for writing into the customer memory is built out of
// the MEM ADR and the all DATA byte
// for indices see user guide. important: index 0

// index 0:
//  measurement time interval, 2 byte in 1/10 seconds, range 15 – 3600 seconds

// ALways very writes by reading values back!

// Example for CRC8 Calculation:
#define CRC8_ONEWIRE_POLY 0x31
#define CRC8_ONEWIRE_START 0xff

class EE894
{
	public:
		EE894(uint8_t id=CO2_DEVICE_ADDR);
		float readTemperature();
		float readHumidity();
		uint16_t readCo2();
        uint16_t readPressure();
		uint16_t readIntervall();
		uint16_t writeIntervall(uint16_t sec);
		void printAll();
	private:
		uint8_t _buf[10];
		uint8_t _id;
    uint8_t i2cCalcCRC8 (uint8_t buf[], uint8_t from, uint8_t to);
		bool read(uint8_t address, uint8_t *data, uint8_t length);
		bool write(uint8_t address, uint8_t *data, uint8_t length);

};

#endif
