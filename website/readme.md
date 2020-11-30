# Sensor Website
Current version: [live](https://critical-sensors.de/) 

Live data [json url](https://critical-sensors.de/srv.php) 

You may filter via parameter *sens=#id*. 
You may specify the period by *days=#number_of_days*. The default period is 14 days, 
to limit processing requirements for the interactive charts. Setting days to zero gives you just the latest item.

Sensor ids: 

  * 0: Reference 
  * 1: Orchard 
  * 2: Orchard 
  * 3: Office
  * 4: ZKM
  * 5: ZKM

[Example](https://critical-sensors.de/srv.php?sens=3&days=0) "https://critical-sensors.de/srv.php?sens=3&days=0" latest record from the office 

```
[{"id":"3",
"req":"1",
"count":"20",
"co2":"501",
"bat":"75",
"pres":"0",
"hum":"27",
"temp":"24.37",
"light":"85",
"rssi":"-256",
"date":"2020-11-30 09:19:34",
"pkt":"1647",
"rep":"0"}]
```

To download all data, use something like [https://critical-sensors.de/srv.php?days=500](https://critical-sensors.de/srv.php?days=500)

Please note, not all sensors are in use all the time

The website might not display all data since Oct 1st,2020, due to processing limitation. 
However, all data can be retrieved via the JSON API.

## Credits 
  * [ZKM](https://zkm.de/de)
  * [OK Lab Karlsruhe](https://ok-lab-karlsruhe.de/)
  * [W3SCHOOLS](https://www.w3schools.com/)
  * [C3JS](https://c3js.org/)
  * [D3JS](https://d3js.org/)
  * [Mohave Font](https://github.com/tokotype/Mohave-Typefaces)
  * [DWD](https://www.dwd.de/DE/leistungen/opendata/opendata.html) German weather service (DWD) reference data
  * [ICOS CO2 Data](https://meta.icos-cp.eu/objects/eJgeOxzxyrBCVW4Xnjr5-WFj)
  * [Scripps Institute](https://scrippsco2.ucsd.edu/data/atmospheric_co2/icecore_merged_products.html) long term CO2 data
  * [PIK-Potsdam](https://www.pik-potsdam.de/de/aktuelles/nachrichten/neue-interaktive-klimagrafiken) inspiration on CO2 concentration display
