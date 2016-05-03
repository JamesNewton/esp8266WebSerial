
/* Serial port Server - ESP8266 Webserver with serial port post and get

Based on ESP8266Webserver, (thank you) Starting from:
https://learn.adafruit.com/esp8266-temperature-slash-humidity-webserver?view=all
 
   Version 1.0  5/3/2016  James Newton at MassMind.org
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#define IOPIN  2 //could replace the debugging prints with an external LED status
 
const char* ssid     = "YourSSID";
const char* password = "YourWifiPassword";
 
ESP8266WebServer server(80); //change to your desired port, 80 is standard http

String webString="";     // String to display
 
void handle_root() {
  //Serial.print(".");
  server.send(200, "text/html", "<html>\
  <head>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <pre><div id='log'></div></pre>\
    <p><form id='msg' action='data' method='get'><input id='txt' name='text' type='text'></input></form></p>\
    <script type='text/javascript' src='http://ajax.googleapis.com/ajax/libs/jquery/1.3/jquery.min.js'></script>\
    <script type='text/javascript'>\
$('#msg').submit(function(){ \
    var clientmsg = $('#txt').val();\
    $.get('data', {text: clientmsg});\        
    $('#txt').attr('value', '');\
    return false;\
  });\
function loadLog(){\
  var log=$('#log');\
  $.ajax({\
      url: 'data',\
      cache: false,\
      success: function(html){\
        log.html(log.html()+html);\
        },\
    });\
  };\
 setInterval (loadLog, 2500);\
      </script>\
  </body>\
</html>");
  delay(100);
}

//Change the setInterval number above for faster or slower updates from serial to web

void setup(void)
{
// You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  uint8_t MAC_array[6];
  char MAC_char[18];
  sprintf(MAC_char,"\n\rMAC:");
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
      sprintf(MAC_char,"%s%02X:",MAC_char,MAC_array[i]);
    } 
//temp uncomment the next line if you need the mac address for your router.
//  Serial.print(MAC_char);
  
// Connect to WiFi network
  WiFi.begin(ssid, password);
//  Serial.print("\n\r \n\rConnecting to");
//  Serial.print(ssid);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
// Uncomment this if you need to find the IP of the ESP from the console
// Better to use fing or your routers dchp logs or to setup manual IP assignment.
//  Serial.println(WiFi.localIP());

  server.on("/", handle_root);

  server.on("/data", [](){  
     if (server.hasArg("text")) {
      Serial.println(server.arg("text"));
      }
    webString="";  
    if (Serial.available()) { webString = Serial.readString(); }
    server.send(200, "text/plain", webString);
  });

//Anything can send data to the serial device by hitting 
// http://ipaddress/data?text=string
// where ipaddress is the address of the esp, and string is the data to send
//Anything can get data by just hitting the /data url. the response will be 
// any serial data recieved since the last connection, in plain text.
 
  server.begin();
//  Serial.println("HTTP server started");
}
 
void loop(void)
{
  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) {
//    Serial.print("!");
    WiFi.begin(ssid, password);
    delay(5000);
    }
} 
 
