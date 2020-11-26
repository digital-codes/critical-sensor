# Critical Sensor Project

## Hardware components
### Sensor Unit
  ![Controller](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/controller-side.jpg)
  
 * M5Stack [ESP32 MicroController](https://docs.m5stack.com/#/en/core/gray)
 
  ![C-bottom](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/controller-bottom.jpg)
  
 * M5Stack [LoRa 868 Module](https://docs.m5stack.com/#/en/module/lora868)
 
  ![Lora](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/lora-bottom.jpg)

 * M5Stack [Light Sensor](https://docs.m5stack.com/#/en/unit/light)
 
  ![Light](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/lightSensor.jpg)

 * EplusE C02-Temerpeature-Humidity-Pressure Sensor [EE894](http://downloads.epluse.com/fileadmin/data/product/ee894/datasheet_EE894.pdf)
 
  ![EE894](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/ee894.jpg)

 * Solar [Power Bank](https://www.pearl.de/a-PX2957-1420.shtml) or similar
 
  ![Power](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/powerBank.jpg)

* Birdhouse style housing, wood or 3D printed

  ![Housing](https://raw.githubusercontent.com/digital-codes/critical-sensor/master/assets/birdhouse-3d.png)

 ### Gateway
  * ESP32 based LoRa Module [TTGO](http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1134&FId=t3:50003:3)
  * Cellular modem based upon [SIM800L](https://www.simcom.com/product/SIM800.html)
  
## Software Components
 * Sensor Software, C++ Arduino based
 * Gateway server, Python, update database with new sensor data
 * Data server, PHP, serving data for website and users
 * Website, HTML, Javascript
   * More infos at website [readme](https://github.com/digital-codes/critical-sensor/blob/master/website/readme.md)
 * Tools, Python for CO2 spiral video and scatterplot matrix
 
 

 
 
