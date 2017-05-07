#include "Arduino.h"
#include "ESP8266WiFi.h"
#include <ESP8266WebServer.h>
#include "WebWiFiScan.h"

#define pinLED BUILTIN_LED
//#define IP_STATIC
#define DEBUG

//dane dla AP
const char* ap_ssid = "AP_WiFiScaner";   // SSID AP
const char* ap_pass = "12345678";  // password do AP
int ap_channel= 2; //numer kanału dla AP


extern ESP8266WebServer server;

#ifdef IP_STATIC
IPAddress IPadr(10,110,2,105); //stały IP
IPAddress netmask(255,255,0,0);
IPAddress gateway(10,110,0,1);
#endif

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif
  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, LOW);
  //ustaw SSID, pass i kanał dla AP
  WiFi.softAP(ap_ssid, ap_pass, ap_channel);
#ifdef IP_STATIC
  WiFi.config(IPadr,gateway,netmask);  // stały IP
#endif
  WiFi.mode(WIFI_AP_STA); //tryb AP_STATION
  WiFi.begin(); 
 int it = 20;
 while ((WiFi.status() != WL_CONNECTED) and it>0) {  //  czekaj na połączenie z WiFi
  delay(500);
   it-- ;
 #ifdef DEBUG
  Serial.print(".");
 #endif
  }
 #ifdef DEBUG 
  if (it>0) {
   Serial.println("");
   Serial.println("WiFi połączone");
   Serial.println(WiFi.localIP());
   Serial.println(WiFi.macAddress());
   Serial.print("Tryb pracy: ");
   Serial.println(WiFi.getMode());
   Serial.print("Tryb modulacji: ");
   Serial.println(WiFi.getPhyMode());
   Serial.print("Nr kanału: ");
   Serial.println(WiFi.channel());
   Serial.print("IP AP: ");
   Serial.println(WiFi.softAPIP() );
   //WiFi.printDiag(Serial);
   }else {
   Serial.println();
   Serial.println("Brak połączenia z WiFi!.");
   }
 #endif
 setservers();
}

void loop()
{
server.handleClient();
if (WiFi.status() != WL_CONNECTED){
  digitalWrite(pinLED, LOW);
  if (WiFi.getMode() != WIFI_AP_STA){
  WiFi.mode(WIFI_AP_STA);}
  }
   else{ // wyłącz LED gdy jest połączenie z WiFi
   digitalWrite(pinLED, HIGH);
  // if (WiFi.getMode() != WIFI_STA){
  // WiFi.mode(WIFI_STA);}
  }
}

