// sensor readout

#include "czSensor.h"
#include "sensors.h"
#include "uCRC16Lib.h"
// need m5stack and wifi for power and htons
#include <M5Stack.h>
#include <WiFi.h>

extern uint16_t loopCnt;


static uint8_t getBatteryState() {
  uint8_t bat;
  // retry battery reads
  for (int i=0;i<20;i++) {
    if ((bat = M5.Power.getBatteryLevel()) != 0xff)
      break;
    delay(50);
  }
  if (M5.Power.isCharging())
    bat |= BATTERY_IS_CHARGING;
  return bat; // 255 if failed
}

// we need to enable power save after all sleep states
// see also https://docs.makerfactory.io/m5stack/api/power/
void powerConfig() {
  Wire.endTransmission(true);  // force I2C end stransmission with stop
  delay(100);
  M5.Power.begin();
  delay(1000);
  Serial.printf("Power control:%d\r\n", M5.Power.canControl());
  //M5.Power.setKeepLightLoad(true); // don't go to sleep
  M5.Power.setPowerBoostKeepOn(true);  // done by deep sleep anyway

  pinMode(LIGHT_DIGITAL_IN, INPUT);
  if (!showLogo) {
	  M5.Lcd.sleep();
	  M5.Lcd.setBrightness(0);
  }
  if (!wifiMode){
    WiFi.mode(WIFI_OFF);
    btStop();
  }

  if (!M5.Power.isCharging()) {
    // check recharging, if not already on and level below 100 and once per day
    if ((getBatteryState() < 100) || (0 == (loopCnt % LOOPS_PER_DAY))) {
      pinMode(POWER_CHARGE_PIN, OUTPUT); // init to input
      digitalWrite(POWER_CHARGE_PIN, 0); // low pulse
      delay(100);
      digitalWrite(POWER_CHARGE_PIN, 1); // reset to 1
      pinMode(POWER_CHARGE_PIN, INPUT); // reset to input
    }
  } else {
    pinMode(POWER_CHARGE_PIN, INPUT); // init to input
  }

  delay(1000);
}


SensorMsg readSensors(){
  SensorMsg sm;

  uint8_t bat = getBatteryState(); // read before sensor!
  // light: read analog
  int light = analogRead(LIGHT_AN_IN);

  // common variables for specific section
  uint16_t t = 0; // temperature
  uint16_t h = 0; // humidity
  uint16_t c = 0; // co2
  uint16_t p = 0; // pressure

  // --------- start specific stuff --------------
  #if (SENSOR_TYPE == SENS_EE894)
  #pragma message "Compiling for sensor EE894"
  #include "ee894.h"
  // we want 12 iteration per loop ~ 300s for 1 hour
  static const uint16_t iv = (uint16_t)(24.0/(float)LOOPS_PER_DAY*3600.0/12.0); // almost 1 hour
  EE894 sensor;
  sensor.printAll();
  // prepare sensor, if required
  // according to vendor median filter has 10 samples. Set sensor to
  // take 12 measurments during 1 loop and read filtered value
  while (sensor.readIntervall() != iv) {
    static int snsLoops = 10;
    sensor.writeIntervall(iv);
    uint16_t x = sensor.readIntervall();
    Serial.printf("Setting intervall, %d -> %d\n",iv,x);
    sensor.printAll();
    if (0 == --snsLoops) break;
  }

  // read sensor
  t = (uint16_t)(sensor.readTemperature() * 100.);
  h = (uint16_t)(sensor.readHumidity() * 100.);
  c = (uint16_t)(sensor.readCo2());
  p = (uint16_t)(sensor.readPressure());
  // reset to long intervall
#endif // EE894

#if (SENSOR_TYPE == SENS_MHZ19)
#pragma message "Compiling for sensor MHZ19"
#include "mhz19.h"
#include "dht12.h"
// we don't use the other ENV sensors here, include anyway
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#error "MHZ19 function undefined"

#endif // MHZ19

#if (SENSOR_TYPE == SENS_SCD30)
#pragma message "Compiling for sensor SCD30"
#include "scd30.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30

// we want 2 iteration per loop to be sure data is available when reading
static const int iv = (int)(24.0/(float)LOOPS_PER_DAY*3600.0/2.0); // almost 1 hour

static SCD30 sensor; // use i2c initialized by power module
// rtc_data_areas survived deep sleep
static RTC_DATA_ATTR bool sensorInitialized = false;

if (sensor.begin(0) == false)
{
  Serial.println("Air sensor not detected. Please check wiring");
}

// we don't know how setting intervall interferes with readout. do it after reading
if (!sensorInitialized) {
  Serial.printf("Init Interval: %d\n",iv);
  delay(20);
  if (false == sensor.setMeasurementInterval(iv))
    Serial.println("Interval error");
  delay(20);
  sensor.setAutoSelfCalibration(true);
  delay(20);
  sensor.beginMeasuring();
  sensorInitialized = true;
} else {
  Serial.println("Sensor already initialized");
}

if (sensor.dataAvailable())
{
  t = (uint16_t)(sensor.getTemperature() * 100.);
  h = (uint16_t)(sensor.getHumidity() * 100.);
  c = (uint16_t)(sensor.getCO2());
  Serial.printf("co2:%d\n",c);
  Serial.printf(" temp:%d\n",t);
  Serial.printf(" humidity:%d\n",h);
} else {
    Serial.println("No data available");
}

#endif // SCD30

  // --------- end specific stuff --------------
  sm.id = deviceId;
  sm.req = SensTypeData | deviceFmt; // add device format
  sm.bat = bat;
  sm.light = (uint8_t) ((LIGHT_MAX_VALUE - light)/41);
  sm.cnt = htons(loopCnt);
  sm.temp = htons(t);
  sm.hum = htons(h);
  sm.co2 = htons(c);
  sm.prs = htons(p);
  sm.crc = htons(uCRC16Lib::calculate((char*)&(sm.id),14)); // compute crc for 14 bytes, skip dummy
  return sm;

}
