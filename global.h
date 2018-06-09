#ifndef GLOBAL_H
#define GLOBAL_H

//ESP8266WebServer server(80);									// The Webserver
AsyncWebServer server(80);

boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
strDateTime DateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long UnixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;											// Counter for Updating the time via NTP
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
byte Minute_Old = 100;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)
HTTPClient http;
//http.setReuse(true); //if server allows it

String cmdStr = ""; //holds incomming IO commands.
boolean havecmd = false; //flag to indicate we have an IO command to process.

/* CONFIGURATION */
#define CONFIG_VER 1

struct strConfig {
  boolean dhcp = true;
  boolean daylight = true;
  long Update_Time_Via_NTP_Every =  60;
  long timezone = -8;
//  byte LED_R;
//  byte LED_G;
//  byte LED_B;
  byte  IP[4]={192,168,1,100};
  byte  Netmask[4]={255,255,255,0};
  byte  Gateway[4]={192,168,1,1};
  char ssid[32] = "attwifi";
  char password[32]="";
  char ntpServerName[64] = "0.pool.ntp.org";
  char streamServerURL[64] = "http://www.massmind.org/techref/getline.asp?"; 

  boolean Logging = false;
  byte WakeCount = 10;
  long baud = 9600;
  boolean Connect = true; //should we enable (optional) RS232 level converter?
  //byte TurnOnHour;
  //byte TurnOnMinute;
  //byte TurnOffHour;
  //byte TurnOffMinute;
  long Interval = 0; //Time to stay asleep. Zero means stay on.
  boolean sleepy = false; //flag to set if Interval > 0. doesn't actually need to be saved!

  char DeviceName[32] = "Not Named"; 
  char datatrigger[10] = "r%0D";
  char dataregexp1[32] = "r%*1crr_%4x0_%4x%*x_%4x"; 
  float dataslope1 = 1;
  float dataoffset1 = 0;
  float dataslope2 = 1;
  float dataoffset2 = 0;
  float dataslope3 = 1;
  float dataoffset3 = 0;
  byte datacount = 0;
  char dataname1[5] = "????"; //5 because terminating null.
  char dataname2[5] = "????";
  char dataname3[5] = "????";
  char pwronstr[10] = "";
  byte pwrondelay = 0;
  } config;
  
void WriteConfig() {
  //EEPROM actually uses SPI_FLASH_SEC_SIZE which appears to be 4096, but we were only allocating 512 bytes on startup
  //because of the way we write the next bit of data past the end of the last here, this is what limited the size of
  //the values. Oh, and ReadStringFromEEPROM only allows 65 characters. TODO: Yike.
  debugln(" Writing Config v", CONFIG_VER);
  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, CONFIG_VER);
  EEPROM.put(3,config);
  EEPROM.commit();
  }

boolean ReadConfig() {
  debug("Reading Config. Found v", EEPROM.read(2));
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == CONFIG_VER ) {
    EEPROM.get(3,config);
    debugln(" loaded v", CONFIG_VER);
    return true;
    }
  else { // DEFAULT CONFIG
    debug(". Replaced w/ default v", CONFIG_VER);
    WriteConfig();
    return false;
  }
}



String get_wifi_status() {
  String state = "N/A";
  String Networks = "";
  if (WiFi.status() == 0) state = "Idle";
  else if (WiFi.status() == 1) state = "NO SSIDs";
  else if (WiFi.status() == 2) state = "SCANNING"; //was SCAN COMPLETE?
  else if (WiFi.status() == 3) state = "CONNECTED: " + WiFi.localIP().toString();
  else if (WiFi.status() == 4) state = "FAILED";
  else if (WiFi.status() == 5) state = "DROPPED";
  else if (WiFi.status() == 6) state = "TRY"; //Was DISCONNECTED
  return state;
}

void ConfigureWifi() {
//https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/readme.md#enable-wi-fi-diagnostic

  debugln("Begin Wifi to point:", config.ssid);
  //debugln("Password:",config.password);
  WiFi.begin (config.ssid, config.password);
  if (!config.dhcp)  {
    debugln(" hardcoded", "");
    WiFi.config(
      IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3] ),
      IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3] ),
      IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3] )
    );
  }
  for (char i = 0; ( ( i < 5) && WiFi.status() != WL_CONNECTED ); i++) {
    delay(1000 * i);
    debug(get_wifi_status(), " "); //this will print our IP if we have one.
  }
  if (WL_CONNECTED == WiFi.status()) {
    AdminEnabled = false; //no need for admin if we are online}
    WiFi.mode(WIFI_STA); //TODO: Re-enable
    debugln("\nAdmin closed. Connect to: ", WiFi.localIP().toString());
  }
  //debugln(" Gateway:",WiFi.gatewayIP());
  //debugln(" localIP:",WiFi.localIP());
}



/*
**
**  NTP
**
*/

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];

void NTPRefresh() {
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress timeServerIP;
    WiFi.hostByName(config.ntpServerName, timeServerIP);
    //sendNTPpacket(timeServerIP); // send an NTP packet to a time server

    debugln("sending NTP packet...","");
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    UDPNTPClient.beginPacket(timeServerIP, 123);
    UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
    UDPNTPClient.endPacket();

    delay(1000);

    int cb = UDPNTPClient.parsePacket();
    if (!cb) {
      debugln("NTP no packet yet", cb);
    }
    else {
      debugln("NTP packet received, length=", cb);
      UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      UnixTimestamp = epoch;
    }
  }
}

void Second_Tick() {
  strDateTime tempDateTime;
  AdminTimeOutCounter++;
  cNTP_Update++;
  UnixTimestamp++;
  ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
  if (config.daylight) // Sommerzeit beachten
    if (summertime(tempDateTime.year, tempDateTime.month, tempDateTime.day, tempDateTime.hour, 0)) {
      ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
    }
    else {
      DateTime = tempDateTime;
    }
  else {
    DateTime = tempDateTime;
  }
  Refresh = true;
}

String parseServer(String response) { //get out any settings and make those changes, leaving just the text to send to the device.
char c;
int p1=0,p2=0;
String text;
  for (int i =0; i < response.length(); i++){
    c=response.charAt(i);
    switch(c) {
      case '\\':
        i++; //don't look at escaped characters
        loop;
      case '{':
        text = response.substring(1,i);
        p1 = i+1; //start of first parameter
        break;
      case ':':
        p2 = i; //end of paramter, start of value
#ifdef DEBUGGING
        debugbuf+=" param:"+response.substring(p1,p2);
#endif
        break;
      case ',':
      case '}':
#ifdef DEBUGGING
        debugbuf+=", val:"+response.substring(p2+1,i);
#endif
        if ( response.substring(p1,p2) == "interval" ) {
          config.Interval = response.substring(p2+1,i).toInt();
          if (config.Interval>0) config.sleepy=true; else config.sleepy=false;
#ifdef DEBUGGING
          debugbuf+="\n interval="+String(config.Interval);
#endif
          }
        if ( response.substring(p1,p2) == "cmd" ) {
          cmdStr=response.substring(p2+1,i);
#ifdef DEBUGGING
          debugbuf+="\n cmd";//="+cmdStr;
#endif
          havecmd=true;
          }
        p1 = i+1; //start of next parameter if it was ','
        break;
      }
    }
  if (0==p1) {text = response;} //if no JASON, entire string is text for device.
  return text;
  }

#endif

