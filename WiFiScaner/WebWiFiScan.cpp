#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// login i hasło do sytemu (dla update)
const char* www_login = "admin";
const char* www_pass = "esp8266";
const char* epass[32];
const char* essid[32];

const char* modes[] = {"NULL","STA","AP","STA+AP"};
const char* phymodes[] = { "","B", "G", "N"};

const int port = 80;                 // port serwera www
ESP8266WebServer server(port);
ESP8266HTTPUpdateServer httpUpdate;

char* IPAdrToStr(int ip)
{
  char* ip_str = new char[16];
  sprintf(ip_str, "%d.%d.%d.%d",
    (ip 	) & 0xFF,
    (ip >> 8) & 0xFF,
    (ip >> 16) & 0xFF,
    (ip >> 24) & 0xFF);
  return ip_str;
}

String HTMLHeader() {           //  nagłówek strony
	String  h = "<!DOCTYPE html>\n";
	  h += "<html>";
	  h += "<head>";
	  h += "<title> WiFi Scaner</title>";
	  h += "<meta charset=\"utf-8\">";
	  h += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	  h += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\" >";
	  h += "</head>";
	  h += "<body style=\"text-align: center;color: white; background: black;font-size: 1.5em;\">\n";
	  return h;
	}

String HTMLFooter() {             //  stopka strony www
	String  f = "";
	  f += "<p>Jan Trzciński &copy; 2017</p></td></tr>";
	  f += "</body>\n";
	  f += "</html>\n";
	  return f;
	}

String HTMLPage1() {      // pierwsza część strony www
	 String t;
	 unsigned long sec = millis() / 1000;
	 unsigned long min = (sec / 60);
	 unsigned long hour = (min / 60);
	 unsigned long days = hour / 24;
	 sec = sec % 60;
	 min = min % 60;
	 hour = hour % 24;
	 t  = "<h1>WiFi skaner</h1>";
	 t += "<p> Wersja oprogramowania 1.2 z dnia 30-03-2017";
	 t += "</p>";
	 t += "<p>Czas od uruchomienia dni: ";
	 t += (days);
	 t += " godz:" ;
	 t += ((hour<10) ? "0" : "");  //gdy mniej niż 10 wstaw zero wiodące
	 t += (hour);
	 t += ":";
	 t += ((min<10) ? "0" : "");  //gdy mniej niż 10 wstaw zero wiodące
	 t += (min);
	 t += ":";
	 t += ((sec < 10) ? "0" : "");  //gdy mniej niż 10 wstaw zero wiodące
	 t += (sec);
	 t += "</p>";
	 t +="<p><a href = \"/wifiscan\"><button class=\"btn btn-info\">Skanuj sieć WiFi</button></a></p>";
	 return t;
	}

String HTMLWiFiScan(void){
	String p="";
	String ix="";
	uint8_t n = WiFi.scanNetworks();

	if (n == 0) return "<p>Brak sieci WiFi.</p>";
	p +="<div><h3>Skanowanie sieci WiFi</h3></div>";
	p +="<table  align=\"center\" border=\"2\" >";
	p +="<thead ><tr><th> </th><th style = \"text-align: center;\">SSID</th>";
	p +="<th>kanał</th><th style = \"text-align: center;\">MAC</th>";
	p +="<th>RSSI</th><th>zabezp</th><th>ukryta</th><tr>";
	p +="</thead><tbody>";
	 for (uint8_t i=0; i<n;i++){
	 p +="<tr><td>";
	 p +="<form action=\"/wifiset\" metod=\"post\">";
	 p +="<labe><input id=\"SSID\" type=\"radio\" name=\"SSID\" value=\""+WiFi.SSID(i)+"\">";
	 p +="</label>";
	 p +="</td>";
	 p +="<td>"+WiFi.SSID(i)+"</td>";	//nazwa sieci
	 p +="<td>"+String(WiFi.channel(i))+"</td>";	//kanał
	 p +="<td>"+WiFi.BSSIDstr(i)+"</td>";	//MAC adres
	 p +="<td>"+String(WiFi.RSSI(i))+" dBm</td>"; //siła sygnału
	 p +="<td>";
	 p +=((WiFi.encryptionType(i)==ENC_TYPE_NONE) ? "nie" : "tak");//czy szyfrowanie
	 p +="</td><td>";
	 p +=((WiFi.isHidden(i)) ? "tak" : "nie");//czy sieć ukryta
	 p +="</td></tr>";
	 } //end for
	 p +="</tbody></table>";
	 p +="<div><p></p> </div>";
	 // gdy połączenie z WiFi
	 if(WiFi.status() == WL_CONNECTED){
		p += "<p>Jesteś połączony z siecią </p>";
		p +="<table align=\"center\" border=\"2\" ><tbody>";
		p +="<thead ><tr><th style = \"text-align: center;\">SSID</th>";
		p +="<th>kanał</th><th style = \"text-align: center;\">MAC</th>";
		p +="<th>RSSI</th></thead><tbody>";
		p += "<td>"+WiFi.SSID()+"</td>";
		p += "<td>"+String(WiFi.channel())+"</td>";
		p += "<td>"+WiFi.BSSIDstr()+"</td>";
		p += "<td>"+String(WiFi.RSSI())+" dBm</td></tbody></table>";
		p +="<label>IP: "+String(IPAdrToStr(WiFi.localIP()))+"</label>";
		p += "<p>Aby zmienić sieć WiFi.</p>";
	 }else{	//gdy brak połączenia z WiFi.
		 p += "<p>Brak połączenia z siecią WiFi.</p>";
		 p += "<label>IP AP: "+String(IPAdrToStr(WiFi.softAPIP()))+"</label>";
	 }	//end if
	 p += "<p>Wybierz sieć powyżej i podaj hasło</p>";
	 p +="<label for=\"password\">Hasło do WiFi:</label>";
	 p +="<input style=\"color: black;\" type=\"password\" name=\"password\" />" ; //required=\"required\"/>";
	 p +="<input style=\"text-align: center;color: black;\" type=\"submit\" value=\"Połącz z WiFi.\"/></form>";
	 p += "<p><a href = \"/wifiscan\"><button class=\"btn btn-info\">Skanuj ponownie</button></a></p>";
	 p += "<p><a href = \"/\"><button class=\"btn btn-info\">Powrót do strony głównej</button></a></p>";
	return p;
}
String WebPageScan(){
	return (HTMLHeader()+HTMLWiFiScan()+HTMLFooter());
}

String WebPage() {       // połącz wszystkie części strony www
 return (HTMLHeader()+ HTMLPage1()+HTMLFooter());
}

// funkcja ustawia wszystkie strony www
void setservers(){
	 httpUpdate.setup(&server,"/update", www_login, www_pass); // umożliwia aktualizację poprzez WiFi

	 server.on("/", [](){
		 if(WiFi.status() == WL_CONNECTED){

		 }
	    server.send(200, "text/html", ((WiFi.status() == WL_CONNECTED)? WebPage():WebPageScan()));
	  });

	 server.on("/wifiscan", [] ()
	  {

		 server.send(200, "text/html", WebPageScan());
	  });
//ustanawia połączenie z wybraną siecią wifi
	 server.on("/wifiset", [] ()
	  {
    //pobierz przysłane dane
		 String ESSID = server.arg("SSID");
		 String Epass = server.arg("password");
    //konwertuj ze string do char
		 const char*	essid = ESSID.c_str();
		 const char* epass = Epass.c_str();
		 WiFi.disconnect() ; //rozłacz obecne połączenie
		 WiFi.mode(WIFI_AP_STA);//ustw tryb AP+STA
		 WiFi.begin(essid,epass); //uruchom łączenie z wybraną siecią wifi
		 delay(200);
     
		 int it = 20; //ustal maksymalny czas czekania na 10sek.(20x500ms)
		 while ((WiFi.status() != WL_CONNECTED and it<0)) {  //  czekaj na połączenie z WiFi
		   delay(500);
		   it--;
		 } 

	 server.send(200, "text/html", WebPageScan());
	  });

	 server.on("/save", []()      // zapamiętaj dane
	  {
	//   saveEEProm();
	   server.send(200, "text/html", WebPage());
	  });

server.begin();                // Start serwera www
#ifdef DEBUG
Serial.println("Server started");
#endif
}
