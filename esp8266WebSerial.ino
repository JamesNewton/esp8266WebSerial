
/* 
esp8266WebSerial
Version: 1.3.1  - 2018-02-12 by James Newton 
Support for ST7735 and ILI9341 and other LCD displays via TFT_eSPI

Version: 1.3.0  - 2017-12-xx by James Newton 
ESPAsyncWebServer for reliable operation
Log incoming data / status to server, stream commands from server out to the connected device
Low power cycle config: sleep for x seconds, wake send data / status to server as available or every x wake cycles.
"Blink Detect" input to watch error lights, etc... on remote devices (with optional photodiode and flipflop for detection during sleep)
Wake without radio for very low power input monitoring
"Pico Jason" interpreter for commands from server to ESP to configure power cycles, logging.

Version: 1.2.0  - 2016-09-xx by James Newton
https://github.com/JamesNewton/esp8266WebSerial

Based on:
  ESP_WebConfig 
  http://www.john-lassen.de/index.php/projects/esp-8266-arduino-ide-webconfig
  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded 
  and that only one request goes to the ESP.


  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public


John Lassen version had problems. First, with the web server operated by polling in the loop,
the little ESP is doing good to deliver one page, but making every click load that page, and the 
styles, and the js, and the icon, and the values is just too much. Many of those issues are 
resolved by moving to ESPAsyncWebServer

It wouldn't be so bad if we could enable casheing of the pages, but that requires a nasty conversion of the time for 
the Last-Modified header value. Actually... maybe it doesn't... it would appear that a fixed, fake, Last-Modified with
a fixed Cache-Control: max-age=### works ok. At least in Chrome.
    response->addHeader ( "Last-Modified", "Wed, 25 Feb 2015 12:00:00 GMT" );  
    response->addHeader ( "Cache-Control", "max-age=86400" );  
Actually, the Last-Modified doesn't seem to be needed... Chrome gets by fine with just max-age. Good enough for now.

Putting each page all in one string would be more reliable, but wastes gobs of memory. 
Or we could make ONE page with everything and sections in tabs:
http://callmenick.com/post/simple-responsive-tabs-javascript-css
that still leaves us with an ajax request for all the data.
  
Building up a single page from multiple constant strings wastes ram and sending page parts is more complex with 
the request->send code because the total content length must be known. But perhaps we can do chunked content? By 
using CONTENT_LENGTH_UNKNOWN and then multiple calls to server.sendContent ala:
http://www.esp8266.com/viewtopic.php?p=34858&sid=e4749ea6b5cca73257f7829adb682f09#p34858
https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/examples/SDWebServer
This requires that we assume HTTP 1.1 clients... shouldn't be an issue.
Why doesn't that code have to close the connection? E.g. how does server know content is finished? Should it
use server.close()? Opened an issue about it. 
https://github.com/esp8266/Arduino/issues/2481

TODO: Looks ESPAsyncWebServer has support for all that already built in:
https://github.com/me-no-dev/ESPAsyncWebServer#template-processing

Added serial data flow from simple web form to serial port and from serial port to web via regular polling.
Debug msgs and startup garbage from bootloader are on the standard serial lines TX GPIO1, RX GPIO3. 
Need to keep actual serial data seperated. Use this serial.swap() to move to different pins, TX GPIO15, RX GPIO13 
and connect /those/ pins to the device.
http://arduino.esp8266.com/versions/1.6.5-1160-gef26c5f/doc/reference.html#serial
the original pins get connected to the bootloader for firmware updates only. 
TODO: Change debug(msg) and debugln(msg) in helpers.h file to check that no serial data is being sent, 
do the swap, serial.send, wait for completion serial.flush, and then swap back. 

DB9
2 BRN TX (driven by logger)
3 RED RX (listened to by logger)
4 ORG
5 YEL GND
  */

/* DISPLAY / DEBUG: Select at most 1 display type. 
No workee... you also have to set it in the TFT_eSPI user_setup_select.h file
*/
//#define TFT_ADAFRUIT_2088 //original 128x128
//#define TFT_ADAFRUIT_358 //new 128x160
#define TFT_ILI9341
#define PIXELS 26
//to drive NeoPixels, SK6812, WS2811, WS2812 and WS2813 with hex P command.
#define DEBUGGING


/* PIN SETUP: Note these are GPIO numbers, NOT "D" number ala NodeMCU
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
*/
//#define SERIAL_ENABLE_PIN 5
#define SERIAL_ENABLE_PIN 10
//AkA CONNECT in web config or F_ON (force on) in schematic. 
//Changed from pin GPIO5 to GPIO10 (aka NodeMCU SD D3 or just SD3) on 02/15/2018
#define WAS_BLINK 4 //D2
#define CLEAR_BLINK 2 //D4 also used for NeoPixels via Serial1 hack
#define TX2 15 //D8
#define RX2 13 //D7
#define CTS_IN 12 //D6
//TFT pins (optional) also uses CLK, SD0(MISO)?, SD1(MOSI), SD3(IO 10), 
#define TFT_DC 14 //D5
#define TFT_CS 0 //D3

//TODO: Actually watch this if customer configures. 
//#define RTS_OUT 14 
//RTS not used or connected to sleep. GPIO14, D5 used as TFT_DC

//TODO: Support user configurable device baud rate
//#define BAUD_RATE 38400
#define BAUD_RATE 9600

//TODO: Support seperate baud rate for debug messages?
#define DEBUG_BAUD_RATE 38400

#define AdminTimeOut 0
//600
// Defines the Time in Seconds, when the Admin-Mode will be diabled
//set to 0 to never disable

#define ACCESS_POINT_NAME  "MassMind.org@192.168.4.1"
/*
Section 7.3.2.1 of the 802.11-2007 specification 
http://standards.ieee.org/getieee802/download/802.11-2007.pdf 
defines a valid SSID as 0-32 octets with arbitrary contents. 
*/
#define ACCESS_POINT_PASSWORD  "192.168.4.1" 
//TODO: Make it respond to ALL ip addresses when in Admin mode or
// pop up a web page automatically on connection ala sign in to WiFi pages.
// ACCESS_POINT_IP 192.168.4.1


#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
//https://github.com/me-no-dev/AsyncTCP   is required by:
#include <ESPAsyncWebServer.h>
//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESP8266HTTPClient.h>
//#include <ESP8266WebServer.h> SUCKS!

//#include <Regexp.h>
//https://github.com/nickgammon/Regexp
//#include "cstdio.h"
//for sscanf

#include <Ticker.h>
//https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

#include <EEPROM.h>
#include <FS.h> //SPIFFS file system
//https://github.com/esp8266/Arduino/blob/master/doc/filesystem.rst

//#include <WiFiUdp.h> //not needed unless we want to support ultra fast UDP 2 way coms.
//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.cpp
//#include "ESP_Sleep.h"

#ifdef TFT_ILI9341
#define TFT_DISPLAY
//#define ILI9341_DRIVER //No workee... you have to set it in the TFT_eSPI user_setup_select.h file
#endif 

#ifdef TFT_ADAFRUIT_358 //Newer 128x160
#define TFT_DISPLAY
//#define ST7735_DRIVER //No workee... you have to set it in the TFT_eSPI user_setup_select.h file
#define TFT_WIDTH 128
#define TFT_HEIGHT 160
#endif 

#ifdef TFT_ADAFRUIT_2088 //original 128x128
#define TFT_DISPLAY
//#define ST7735_DRIVER //No workee... you have to set it in the TFT_eSPI user_setup_select.h file
#define TFT_WIDTH 128
#define TFT_HEIGHT 128
#endif 

#ifdef TFT_DISPLAY
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#define TFT_SPI_OVERLAP
//#define TFT_CS   PIN_D3  // GPIO 0
//#define TFT_DC   PIN_D5  // GPIO 14 Data Command control pin

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm

/*
https://github.com/Bodmer/TFT_eSPI Supports using the SPI0 bus:
https://github.com/Bodmer/TFT_eSPI/issues/74

The exact type of display must be set in \Arduino\libraries\TFT_eSPI-master by updating the
User_Setup_Select.h files and then the file selected there.

Requires SPI.pins which came in core 2.4.0, but can be added to 2.3.0 by updating the SPI core library
https://github.com/esp8266/Arduino/tree/af0f5ed956b85e41da647d51d1799db469d6e697/libraries/SPI and also:
https://github.com/esp8266/Arduino/blob/af0f5ed956b85e41da647d51d1799db469d6e697/cores/esp8266/esp8266_peri.h
These files live in 
C:\Users\JamesN2\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.3.0 and libraries\SPI under that
to recompile the core library, you must fine the spi.o file in the temp folder used for output and delete it

https://www.adafruit.com/product/2088
http://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
https://learn.adafruit.com/adafruit-gfx-graphics-library?view=all
 */
#endif

#ifdef PIXELS
//#include <NeoPixelAnimator.h>
//#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PIXELS, 2); //number of pixels, pin must be GPIO2, TX1
#endif

#include "base64.h"
extern "C" {
#include <user_interface.h> //Required for getResetInfoPtr()
}

#ifdef DEBUGGING
String debugbuf = "";
#ifdef TFT_DISPLAY
int debug_y;
int debug_x;
#define TFT_DEBUG_START 22
#define TFT_DEBUG_END TFT_HEIGHT-16
#endif
#endif

#include "helpers.h"
#include "global.h"


/* Include the HTML, STYLE and Script "Pages" */
#include "Page_Root.h"
#include "Pages.h"
#include "Page_Style.css.h"
#include "Page_Script.js.h"
#include "example.h"

#define STREAM_BUF_LINES 5
#define STREAM_MAX_CONFIDENCE 10

//extern "C" int __get_rf_mode(void); causes "unknown function error"
#define MICROSECONDS 1000000
//ADC_MODE(ADC_VCC); //include to measure 3.3 volt rail. comment out to use ADC pin. 

/****** Global Variables *****/

//anything defined inside this struct will be saved to RTC memory on DeepSleep and should survive the restart
//these variables can change constantly and will not wear out the eeprom.
struct {
  float count; //TODO: why is this a float?
  bool RFon;
} rtcmem;
static_assert (sizeof(rtcmem) < 512, "RTC can't hold more than 512 bytes");
rst_info *reset;

String DeviceID = "????????"; //compressed unique id for logging to server. 
//Will be made from MAC ID Base64 Encoded with URL safe character set

byte streaming=0;
String streamURL = "";
int streamLine = 1;
String streamBuf[STREAM_BUF_LINES];
int streamBufLine = 0;

boolean xoff=false; //flag to see if we need to hold off xmit until device is ready
boolean havedata = false; //flag to indicate data in rxbuf so we don't have to check length each loop.
String rxbuf = ""; //buffer for data recieved from device.
#define RFBUF_MAX 128 

int reading1,reading2,reading3; //extracted value from datastream for local display.
short readingcount = 0;

//If Xoff, recieve bytes until Xon before sending byte.
void putc_x(byte b) {
  while (Serial.available() || xoff) {
    byte c = Serial.read(); 
    //can't peek, because xoff could be behind next char
    if (c == 0x13) { xoff=true; } // XOFF
    else if (c == 0x11) { xoff=false; } // XON
    else if (c > 0 && c < 0xFF) { //rx data
      rxbuf += c; //buffer it
      //TODO: may need to check rxbuf.length() and send an xoff if too big.
      havedata=true;
      }
    delay(1); //should this be more?
    //TODO: timeout?
    }
  Serial.write(b);
  }


//If Xoff, recieve bytes until Xon before sending string.
void writeStr_x(String msg) {
  for(int i=0;i<msg.length();i++) {
     putc_x(msg[i]);
     };
  }

boolean checkSerial(byte timout) {
  while (Serial.available()) { //note there is no timeout delay if there is no serial data waiting
    char c = Serial.read();  //gets one byte from serial buffer
    if (c == 0x13) { xoff=true; } // XOFF
    else if (c == 0x11) { xoff=false; } // XON
    else if (c>0 && c<0xFF) { rxbuf += c; havedata=true;} //filter out nulls and FF's.
    if (!Serial.available() && timout) { delay(timout); } //wait a tich if there isn't already more data available. Otherwise, timeout.
    }
//  regexp.Target(rxbuf);
//  if (REGEXP_MATCHED == regexp.Match(config.regexp) {
//    reading1 = hex2int(regexp.GetMatch(rxbuf,0); //extract it.
//  }
//  int i = rxbuf.length();
//  if ( ('+'  == rxbuf[i-8]) || ('-' == rxbuf[i-8]) ) { //detected a reading?
//    reading1 = hex2int(rxbuf.substring(i-8)); //extract it.
//    //debug("hexvalue:",reading1);
//    rxbuf = ""; //TODO: only dump it if valid data and user setting allows.
//    }

  }

#ifdef TFT_DISPLAY
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
#define TFT_BACKGROUND TFT_BLUE
#define TFT_RBG_COLOR(r,g,b) (((((r)/8)&31) << 11) | ((((g)/4)&63) << 5) | (((b)/8)&31))

void showreading(String reading, int x, int y, int fontno) {
  tft.setTextColor(TFT_YELLOW,TFT_BACKGROUND); 
  tft.setTextFont(fontno); tft.setTextSize(1);
  if (x<0) { 
    x = -x - tft.textWidth(reading,fontno) - 2;
    if (x<0) x = 0;
    }
  tft.fillRect(0,y,TFT_WIDTH,tft.fontHeight(fontno),TFT_BACKGROUND);
  tft.setCursor(x, y); 
  tft.print(reading);
  }
  
#endif

void setup ( void ) {
  EEPROM.begin(512); //EEPROM actually uses SPI_FLASH_SEC_SIZE which appears to be 4096
  Serial.begin(BAUD_RATE); //take a guess, so we can see debug messages from ReadConfig
  Serial.swap(); //change to TX GPIO15, RX GPIO13 
  //pinMode(TX2, OUTPUT); //TX line to device should always be an output
  //digitalWrite(TX2, HIGH); //when not used as serial, keep TX high or device will see nulls or FF's
  //pinMode(RX2,INPUT_PULLUP); // when not used as serial, keep RX high or we may see nulls or FF's
  pinMode(SERIAL_ENABLE_PIN, OUTPUT);
  digitalWrite(SERIAL_ENABLE_PIN, LOW);
  pinMode(WAS_BLINK, INPUT);
  digitalWrite(CLEAR_BLINK, HIGH);
  pinMode(CLEAR_BLINK, OUTPUT);

  debug("\r","Start ES8266 reason:");
  
  reset = ESP.getResetInfoPtr();
  debugln(reset->reason,ESP.getResetReason());
  switch (reset->reason) {
    case REASON_EXT_SYS_RST: //6
    case REASON_DEFAULT_RST: //0
    //standard power up or reset
      rtcmem.count=0;
      rtcmem.RFon=true; //TODO: Is this right? Will the radio always come on by default?
      break;
    case REASON_DEEP_SLEEP_AWAKE: // 5
    //wake up from RTC
      ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcmem, sizeof(rtcmem));
      rtcmem.count++;
      debugln(" for RTC Tick ",rtcmem.count);
      break;
    }

  ReadConfig(); //returns false and sets up default config if none found. See global.h
  config.Interval = 0; //Temporary: Use to break out of sleep loop.
  debugln(config.baud," baud");
  config.baud = 9600; //temporary 
  Serial.swap(); //change back
  Serial.begin(config.baud);
  Serial.swap(); //change to TX GPIO15, RX GPIO13 
  
  if (config.Interval > 0) config.sleepy=true; else config.sleepy=false;
  if (rtcmem.count >= config.WakeCount) {
    havedata = true; //fake having data so we will connect. 
    debugln("Check in",config.WakeCount);
    }
// Check if we really need to wake up, and if we don't, just go back to sleep
  if ( !digitalRead(WAS_BLINK)   //havent see a blink
    && config.Logging //and we are logging, but
    && config.sleepy  //we are setup to go to sleep //config.Interval>0 causes webserver issues here
    && !havedata      //not waiting on data to be logged to the server //rxbuf.length()==0 causes webserver issues here
    ) {               //then lets go to sleep
    debugln("Sleep for ",config.Interval);
    rtcmem.RFon = false;
    ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcmem, sizeof(rtcmem)); //write data to RTC memory so we don't loose it.
    ESP.deepSleep(config.Interval * MICROSECONDS, WAKE_RF_DISABLED);
    //WAKE_NO_RFCAL); //deep sleep, shut off RF, wake back up in setup.
    //https://github.com/esp8266/Arduino/issues/3072
    }
//If we get to here, we want to wake up and talk, but the radio might not be on. Only way to turn it on is to set a flag and go back to sleep.
  if (!rtcmem.RFon) {
    debugln("Reset for RF","");
    rtcmem.RFon = true;
    ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcmem, sizeof(rtcmem)); //write data to RTC memory so we don't loose it.
    ESP.deepSleep(1, WAKE_NO_RFCAL); //deep sleep, wake right away with RF on, assume calibration not needed
    }
//we are up and radio should be on.

#ifdef TFT_DISPLAY
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BACKGROUND);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE,TFT_BACKGROUND); tft.setTextFont(1); tft.setTextSize(1);
  #ifdef DEBUGGING
  debug_x=1; debug_y=TFT_DEBUG_START;
  #endif
#endif
#ifdef PIXELS
  strip.Begin();
  strip.Show();
#endif

  if (config.Connect) {
    digitalWrite(SERIAL_ENABLE_PIN, HIGH);
    debugln("Device port enabled","");
    }
  else {
    debugln("Device port disabled","");
    }
  readingcount = config.datacount;
  
//uncomment the next line if you need the mac address for your router.
  debugln("MAC:",GetMacAddress());
  uint8_t mac[6];
  WiFi.macAddress(mac);
  DeviceID = base64::encode(mac,6);
  DeviceID.replace("+","-");
  DeviceID.replace("/","_");
  debugln("DeviceID:",DeviceID);
#ifdef TFT_DISPLAY
  tft.print("MAC:");
  tft.println(GetMacAddress());
  tft.print("Device:");
  tft.print(DeviceID);
#endif


	if (AdminEnabled)	{
		//WiFi.mode(WIFI_AP_STA);
    WiFi.mode(WIFI_AP);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    debugln("Connect to SSID:",ACCESS_POINT_NAME);
    debugln("Password:",ACCESS_POINT_PASSWORD);
    debugln("http://",myIP);
	  }
	else	{
		WiFi.mode(WIFI_STA);
	}
 
	ConfigureWifi();
	SPIFFS.begin(); 
  server.on ( "/", HTTP_GET, handle_root  ); //main web page in Page_Root.h
	server.on ( "/admin.html", HTTP_GET, [](AsyncWebServerRequest *request) { //settings menu
#ifdef DEBUGGING
	  debugbuf += "admin.html";
#endif
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", PAGE_AdminMainPage); //in Pages.h
    response->addHeader ( "Cache-Control", "max-age=86400" );  //doesn't change, let the browser know
    //response->addHeader ( "Last-Modified", "Wed, 25 Feb 2015 12:00:00 GMT" );  //doesn't appear to work
	  request->send ( response );   
	  }  );
  server.on ( "/general.html", HTTP_GET, send_general_html  ); //in Pages.h
  server.on ( "/general.html", HTTP_POST, send_general_html  ); //ESPAsyncWebServer needs separate GET and POST handlers
  server.on ( "/device.html", HTTP_GET, send_device_html  ); //in Pages.h
  server.on ( "/device.html", HTTP_POST, send_device_html  ); //in Pages.h
  server.on ( "/update", HTTP_GET, send_fs_html  ); //in Pages.h
  //server.onFileUpload(handle_fs_upload); //in Pages.h
  server.on ( "/update", HTTP_POST, [](AsyncWebServerRequest *request){
      request->send(200);
    }, handle_fs_upload  ); //in Pages.h
  server.on ( "/config.html", HTTP_GET, send_network_configuration_html );
  server.on ( "/config.html", HTTP_POST, send_network_configuration_html );
	server.on ( "/info.html", HTTP_GET, [](AsyncWebServerRequest *request) { 
#ifdef DEBUGGING
    debugbuf += "/info.html"; 
#endif
	  request->send ( 200, "text/html", PAGE_Information );
	  } );
  server.on ( "/ntp.html", HTTP_GET, send_NTP_configuration_html  );
  server.on ( "/ntp.html", HTTP_POST, send_NTP_configuration_html  );

	server.on ( "/example.html", HTTP_GET, [](AsyncWebServerRequest *request) { request->send ( 200, "text/html", PAGE_example );  } );
  server.on ( "/admin/filldynamicdata", HTTP_GET, filldynamicdata );

  server.on ( "/favicon.ico", HTTP_GET, send_favicon_ico ); //in Page_Root.h
	server.on ( "/style.css", HTTP_GET, [](AsyncWebServerRequest *request) { 
#ifdef DEBUGGING
	  debugbuf += "/style.css"; 
#endif
    AsyncWebServerResponse *response = request->beginResponse( 200, "text/css", PAGE_Style_css );
    response->addHeader ( "Cache-Control", "max-age=86400" );  
    request->send ( response );   
	  } );
	server.on ( "/microajax.js", HTTP_GET, [](AsyncWebServerRequest *request) { 
#ifdef DEBUGGING
	  debugbuf+="/microajax.js"; 
#endif
    AsyncWebServerResponse *response = request->beginResponse( 200, "text/plain", PAGE_microajax_js );
    response->addHeader ( "Cache-Control", "max-age=86400" );  
    request->send ( response );   
	  } );
	server.on ( "/admin/values", HTTP_GET, send_network_configuration_values_html );
	server.on ( "/admin/connectionstate", HTTP_GET, send_connection_state_values_html );
	server.on ( "/admin/infovalues", HTTP_GET, send_information_values_html );
	server.on ( "/admin/ntpvalues", HTTP_GET, send_NTP_configuration_values_html );
	server.on ( "/admin/generalvalues", HTTP_GET, send_general_configuration_values_html);
	server.on ( "/admin/devicename", HTTP_GET, send_devicename_value_html);
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){  
    if (request->hasArg("text")) {
      writeStr_x(parseServer(request->arg("text"))); //pass on what was sent less server msgs
      Serial.flush(); //complete the send before going on
      rxbuf=""; havedata=false; //done with that data. TODO: Why are we doing this here?
      //delay(10); //give the device time to respond //nope, can't 'cause AsyncWebServer, get it next browser request
      checkSerial(0); // get any text that comes back now //parameter is delay, must be 0 with AsyncWebServer
      }
    request->send(200, "text/html", rxbuf); //should be text/plain but for testing... 
#ifdef DEBUGGING
    if (rxbuf.length()>0) debugbuf+=">"+rxbuf;
#endif
    if (!config.Logging) {rxbuf=""; havedata=false;} //don't clear if logging so the server gets a copy
    });
  server.on("/file", HTTP_GET, [](AsyncWebServerRequest *request){  
    if (request->hasArg("start")) { //have to specify a starting line to begin streaming.
      if (WL_CONNECTED != WiFi.status()) {
#ifdef DEBUGGING
        debugbuf+="file requested, but no net\n";
        debugbuf+=get_wifi_status();
#endif
        request->send(400, "text/html", "No internet access, check network config"); 
        }
      else { //connected, start new file
        streamLine = request->arg("start").toInt(); // || 1; //the logical or 1 doesn't work for some reason!!!
        if (streamLine < 1) streamLine = 1;
        streamURL = config.streamServerURL + request->arg("name"); //include file name if specified
        http.begin(streamURL + "&line=" + streamLine);
#ifdef DEBUGGING
        debugbuf+="Streaming from \n";
        debugbuf+=streamURL;
        debugbuf+="&line=";
        debugbuf+=streamLine;
        debugbuf+="\n";
#endif
        //TODO: Make the argument name for the line number configurable
        //maybe it should be post data?
        int httpCode = http.GET();
        if (HTTP_CODE_OK==httpCode) {
          if (streaming < STREAM_MAX_CONFIDENCE) streaming++; //flag it so the loop will keep running
          streamBuf[streamBufLine] = http.getString(); //first line returned on open
          request->send(200, "text/html", (String)"Streaming:"+streamBuf[streamBufLine]+"<a href=\"/file?stop=\">Stop</a> <a href=\"/file\">Status</a>"); 
          streamBufLine++;
          streamLine++;
          }
        else {
          request->send(400, "text/html", http.errorToString(httpCode)+"\nBad response from file stream server, check network config."); 
          }
        http.end();
        }
      } //done with new stream
    else if (request->hasArg("stop")) { 
      streaming = 0;  //stop the cha loop a
      request->send(200, "text/html", (String)"Streaming halted at line:" + streamLine + " <a href=\"/file?start=" + streamLine + "\">Continue</a>"); 
      }
    else { //got a request but no start
      if (streaming) { //already streaming, just provide a status update.
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (String)"Streaming:" + streamLine + +"\n<br><a href=\"/file?stop=\">Stop</a>");
        response->addHeader ( "Refresh", "5" );  // short version of <META HTTP-EQUIV="Refresh" CONTENT="600">
        request->send(response); 
        }
      }
    });
	server.onNotFound ( [](AsyncWebServerRequest *request) { 
#ifdef DEBUGGING
	  debugbuf+="404:";
	  debugbuf+=request->url(); 
    debugbuf+="\n";
#endif
	  request->send ( 404, "text/html", "Page not Found" );
	  }  );
  server.serveStatic("/", SPIFFS, "/").setTemplateProcessor(send_tag_values);
	server.begin();
	debugln("HTTP server started on port:","80" );
  debugln("Response Length:", strlen(PAGE_AdminMainPage));
	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive
#ifdef TFT_DISPLAY
  tft.println(" up");
#endif

  } //Setup



void loop ( void ) {
	if (AdminEnabled && (AdminTimeOut>0))	{
		if (AdminTimeOutCounter > AdminTimeOut) {
			 AdminEnabled = false;
			 debugln("Admin Mode disabled!","");
			 WiFi.mode(WIFI_STA);
      //WiFi.disconnect();
      //WiFi.softAPdisconnect(true);
		  }
	  }
   
	if(DateTime.minute != Minute_Old)	{ //only check once a minute
		Minute_Old = DateTime.minute;
#ifdef DEBUGGING
     //Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
    debug("mem:",ESP.getFreeHeap());
    debug(" ",DateTime.year);
    debug("/",DateTime.month);
    debug("/",DateTime.day);
    debug(" ",DateTime.hour);
    debug(":",DateTime.minute);
    debug(":",DateTime.second);
    if (AdminEnabled) {debug(" admin ","")};
    if (config.Logging) {debug(" log ","")};
    if (config.sleepy) {
      debug(" sleep for:",config.Interval)
      debug(" checkin every:",config.WakeCount)
      };
    if (havedata) {debug(" data:",rxbuf)};
    debugln("","");
#endif
#ifdef TFT_DISPLAY
    if (WL_CONNECTED == WiFi.status()) { 
      tft.setCursor(2, 2);//leave room for power status
      tft.setTextColor(TFT_YELLOW,TFT_BACKGROUND); tft.setTextFont(2); tft.setTextSize(1);
      IPAddress ip = WiFi.localIP();
      tft.print(String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + ".");
      tft.setTextFont(1); tft.setTextSize(2); 
      tft.println(String(ip[3]));
      }
    else if (AdminEnabled) {
      tft.setCursor(1, 2);//leave room for power status
      tft.setTextColor(TFT_WHITE,TFT_BACKGROUND); tft.setTextFont(1); tft.setTextSize(1);
      tft.print("MAC:");
      tft.println(GetMacAddress());
      tft.setTextColor(TFT_YELLOW,TFT_BACKGROUND); tft.setTextFont(1); tft.setTextSize(1);
      tft.print("SSID:");
      tft.println(ACCESS_POINT_NAME);
      tft.print("PASS:");
      tft.println(ACCESS_POINT_PASSWORD);
      };
#endif
    if (config.Update_Time_Via_NTP_Every  > 0 ) {
      if (cNTP_Update > 5 && firstStart) {
        NTPRefresh();
        cNTP_Update =0;
        firstStart = false;
        }
      else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) ) {
        NTPRefresh();
        cNTP_Update =0;
        }
      }
    }

  if (streaming) {
    if (streamBufLine>0 && !xoff) {
      streamBufLine--;
      debugln("<",streamBuf[streamBufLine]);
      writeStr_x(streamBuf[streamBufLine]);
      }
    if (STREAM_BUF_LINES>streamBufLine) { //we have room to buffer more lines.
      if (WL_CONNECTED != WiFi.status()) {
        debugln("net lost while streaming:", get_wifi_status());
        streaming = 0; //stop the cha loop a
        }
      else {
        //streamURL = config.streamServerURL + server.arg("name"); //already set in server.on("/file"
        //http.begin(streamURL + "&line=" + streamLine + "&lines=" + (STREAM_BUF_LINES-streamBufLine));
        //TODO: Deal with multiple line returns.
        http.begin(streamURL + "&line=" + streamLine + "&data=" + rxbuf );
        //TODO: Make the argument name for the line number configurable
        //maybe it should be post data?
        int httpCode = http.GET(); //blocking TODO: Timeout?
        if (HTTP_CODE_OK==httpCode) {
          //debugln(">",streamBuf[streamBufLine]);
          streamBuf[">",streamBufLine] = http.getString();
          streamBufLine++;
          streamLine++;
          if (streaming < STREAM_MAX_CONFIDENCE) streaming++;
          debugln(">",rxbuf);
          rxbuf="";
          havedata=false;
          }
        else { //TODO: Stop instantly on 404.
          debug(http.errorToString(httpCode),httpCode);
          debugln(" at line:",streamLine);
          streaming--; //loosen grip on the cha loop a
          }
        http.end();
        }
      }      
    }
    
  checkSerial(1); 
//http://www.cplusplus.com/reference/cstdio/scanf/ 
  if (0<sscanf(rxbuf.c_str(),config.dataregexp1,&reading1,&reading2,&reading3)) {
    rxbuf="";
    debugbuf+="^"; 
    }


  if ( ( config.Logging && ( havedata || digitalRead(WAS_BLINK) ) )
  //http://www.massmind.org/techref/getline.asp?id=GP40noFt&line=200&lines=1000
  ) {
    streamURL = config.streamServerURL;
    if (!http.begin(streamURL + "id=" + DeviceID + "&data=" + urlencode(rxbuf.c_str()) + "&blink=" + (String) (digitalRead(WAS_BLINK)? "YES": "NO") ) ) {
      debug("Failed to open ",streamURL + "id=" + DeviceID + "&data=" + urlencode(rxbuf.c_str()));
      }
    else {
      int httpCode = http.GET(); //blocking TODO: Timeout?
      if (HTTP_CODE_OK==httpCode) {
        //TODO: Server response can set sleep interval and send data to device.
        debug("logged:",rxbuf);
        if (digitalRead(WAS_BLINK)) debug("BLINK ","");
        rxbuf=http.getString();
        debugln("Response:",rxbuf);
        //TODO: If we got a command to send to the device, initiate the connection
        havedata = false; //assume 
        if ( rxbuf.length()>0 ) {
          havedata=true; //correct
          rxbuf = parseServer(rxbuf); //get out any settings and make those changes, leaving just the text to send to the device.
          debugln("send:",rxbuf);
          if ( rxbuf.length()>0 ) {
            digitalWrite(SERIAL_ENABLE_PIN, HIGH);
            delay(5); //give the RS232 transceiver / level converter time to respond
            writeStr_x(rxbuf); //pass on the servers response
            Serial.flush(); //complete the send before going on
            rxbuf=""; havedata=false; //done with that data.
            delay(10); //give the device time to respond
            checkSerial(10); // get any text that comes back now
            }
          }
        if (rtcmem.count >= config.WakeCount) {rtcmem.count = 0;} //reset checkin count.
        digitalWrite(CLEAR_BLINK, LOW); //disable any further blinks while we are awake.
        pinMode(CLEAR_BLINK, OUTPUT); //incase it was overwritten by Serial1 NeoPixel cmdString
        //TODO: Don't we want to see additional blinks?
        }
      else { 
        debug("Logging failed to ",streamURL);
        debug("id=", DeviceID);
        debug("&data=", urlencode(rxbuf.c_str()));
        debug("error:",http.errorToString(httpCode));
        debugln(" errorcode:",httpCode);
        debugln(" log:",http.getString());
        delay(1000);
  // if we aren't connected to a browser, rxbuf will overflow
  // TODO: try to log rxbuf to SPIFFs if a server isn't reachable.
  // TODO: At least just dump all but the last x characters. 
  // HACK: For now, just dump 
        rxbuf="";
        }
      http.end();
      }
    }

  if (havecmd) {
    havecmd = false;

    debug(cmdStr.length(),"chars");
    debugln(" cmd:", cmdStr);
#ifdef PIXELS
    //https://github.com/Makuna/NeoPixelBus/blob/master/src/internal/NeoEsp8266UartMethod.cpp
    uint16_t pixno;
    uint8_t redness,greenness,blueness;
    debug(" pixels",cmdStr.length()>>2);
    //http://192.168.0.127/data?text={cmd:aF84bF84cF84dF84eF84fF84gF84hF84iF84jF84kF84lF84mF84nF84oF84pF84qF84rF84sF84tF84uF84vF84wF84xF84yF84}
    //http://192.168.0.127/data?text={cmd:yF84xF84wF84vF84uF84tF84sF84rF84qF84pF84oF84nF84mF84lF84kF84jF84iF84hF84gF84fF84eF84dF84cF84bF84aF84}
    //http://192.168.0.127/data?text={cmd:a48Fb48Fc48Fd48Fe48Ff48Fg48Fh48Fi48Fj48Fk48Fl48Fm48Fn48Fo48Fp48Fq48Fr48Fs48Ft48Fu48Fv48Fw48Fx48Fy48F}
    while( cmdStr.length()>=4) {
      pixno = cmdStr[0]-'a';
      redness = h2int(cmdStr[1])<<4;
      greenness = h2int(cmdStr[2])<<4;
      blueness = h2int(cmdStr[3])<<4;
      debug(" #",pixno);
      debug(" R",redness);
      debug(" G",greenness);
      debugln(" B",blueness);
      strip.SetPixelColor(pixno, RgbColor(redness, greenness, blueness) );
      //strip.SetPixelColor(h2int(cmdStr[0]), RgbColor(h2int(cmdStr[1]),h2int(cmdStr[2]),h2int(cmdStr[3])) );
      cmdStr = cmdStr.substring(4);
      } //done with pixels
    strip.Show();
#endif
    cmdStr="";
    } //done with cmd
  
	if (Refresh)  { //Refresh gets set once a second by Second_Tick() in global.h
		Refresh = false; //service it here when we get a round toit.
    if (!AdminEnabled   //not in admin mode
      && config.Logging //and we are logging, but
      && config.sleepy  //we are setup to go to sleep //config.Interval>0 causes webserver issues here
      && !havedata      //not waiting on data to be logged to the server //rxbuf.length()==0 causes old webserver issues here
      ) {               //then lets go to sleep
        debugln("Sleep for ",config.Interval);
        digitalWrite(CLEAR_BLINK, HIGH); //allow new blinks to be detected
        pinMode(CLEAR_BLINK, OUTPUT); //incase it was overwritten by Serial1 NeoPixel cmdString

        //TODO: Drop the connection to the device.
        ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcmem, sizeof(rtcmem)); //write data to RTC memory so we don't loose it.
        //TODO: Compensate for how long we have already been awake. e.g. (Interval - DateTime.Seconds)
        ESP.deepSleep(config.Interval * MICROSECONDS, WAKE_NO_RFCAL); //deep sleep, assume RF ok, wake back up in setup.
      }
    else {
//      debugln(config.sleepy?"1":"0",havedata?"1":"0");
#ifdef TFT_DISPLAY
      if (config.datacount && !readingcount--) {
        readingcount = config.datacount;
        writeStr_x(config.datatrigger);
        }
#endif
      }

#ifdef DEBUGGING
    if (debugbuf.length()>0) {
      debugln(debugbuf,"");
  #ifdef TFT_DISPLAY
      tft.setCursor(debug_x, debug_y);
      tft.setTextColor(TFT_WHITE,TFT_BLUE);    tft.setTextFont(1); tft.setTextSize(1);
      tft.println(debugbuf);
      debug_x = tft.getCursorX();
      debug_y = tft.getCursorY();
      if (debug_y > TFT_DEBUG_END) {
        debug_x=1; debug_y=TFT_DEBUG_START;
        }
  #endif
      debugbuf="";
      }
#endif
#ifdef TFT_DISPLAY
    if (config.datacount) { //don't mess with this if we aren't updating the local display.
      tft.drawLine(0, 0, 128, 0, TFT_BLACK); //erase the prior reading
      //tft.drawLine(0, 0, (int)(ESP.getVcc()/50), 0, TFT_RBG_COLOR(0,0,255)); //read power rail. Needs ADC_MODE(ADC_VCC);
      tft.drawLine(0, 0, (int)(analogRead(A0)*TFT_WIDTH/1024), 0, TFT_RBG_COLOR(0,0,255)); //Read analog input. 0-1.0 volts
      tft.drawLine(0, 1, TFT_WIDTH, 1, TFT_BLACK); //erase the prior reading
      tft.drawLine(0, 1, (int)random(80), 1, TFT_RBG_COLOR(255,0,0)); //TODO: Replace with something useful
      showreading(String(config.dataslope1*(float)reading1+config.dataoffset1)+config.dataname1,-TFT_WIDTH,TFT_HEIGHT/3,4);
      showreading(String(config.dataslope2*(float)reading2+config.dataoffset2)+config.dataname2,-TFT_WIDTH,TFT_HEIGHT/2,4);
      showreading(String(config.dataslope3*(float)reading3+config.dataoffset3)+config.dataname3,-TFT_WIDTH,TFT_HEIGHT/1.5,4);
      }
#endif
    }


  } // main loop


