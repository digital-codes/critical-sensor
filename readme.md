# Critical Sensor Project

## Hardware components
### Sensor Unit
 * M5Stack [ESP32 MicroController](https://docs.m5stack.com/#/en/core/gray)
 * M5Stack [LoRa 868 Module](https://docs.m5stack.com/#/en/module/lora868)
 * M5Stack [Light Sensor](https://docs.m5stack.com/#/en/unit/light)
 * EplusE C02-Temerpeature-Humidity-Pressure Sensor [EE894](http://downloads.epluse.com/fileadmin/data/product/ee894/datasheet_EE894.pdf)
 * Birdhouse style housing, wood or 3D printed
 
 ### Gateway
  * ESP32 based LoRa Module [TTGO](http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1134&FId=t3:50003:3)
  * Cellular modem based upon [SIM800L](https://www.simcom.com/product/SIM800.html)
  
## Software Components
 * Gateway server, Python, update database with new sensor data
 * Data server, PHP, serving data for website and users
 * Website
 

 
 
