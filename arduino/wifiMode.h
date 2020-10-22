// lora interface
#ifndef WIFI_MODE_H
#define WIFI_MODE_H

#include <WiFi.h>

//#define WIFIMULT

#ifdef WIFIMULT
// for wifimult
#include <WiFiMulti.h>
#define NUM_APS 2
const char *wifi_ssid[] = {"karlsruhe.freifunk.net","IOT"}
const char *wifi_pwd[] = {"",""};
#else

/*
const char* ssid     = "IOT";
const char* password = "1234abcd";
*/
/* */
const char* ssid     = "karlsruhe.freifunk.net";
const char* password = "";
/* */
/*
const char* ssid     = "akugel_g";
const char* password = "";
*/

#endif

// fill these 2 positions before compiling
const char* host = "add your own host here  ";
const int port = add your own port here;


#endif // WIFI_MODE_H
