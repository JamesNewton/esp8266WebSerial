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

/* CONFIGURATION */

struct strConfig {
  boolean dhcp;
  boolean daylight;
  long Update_Time_Via_NTP_Every;
  long timezone;
  byte LED_R;
  byte LED_G;
  byte LED_B;
  byte  IP[4];
  byte  Netmask[4];
  byte  Gateway[4];
  String password;
  String ssid;
  String ntpServerName;

//  boolean AutoTurnOn;
  boolean Logging;
//  boolean AutoTurnOff;
  byte WakeCount;
  //byte TurnOnHour;
  boolean Connect;
  //byte TurnOnMinute;
  //byte TurnOffHour;
  //byte TurnOffMinute;
  long Interval; //time to stay asleep
  boolean sleepy; //flag to set if Interval > 0. NOT actually saved!

  String DeviceName;
  String streamServerURL;
  } config;

void WriteConfig() {
  //EEPROM actually uses SPI_FLASH_SEC_SIZE which appears to be 4096, but we were only allocating 512 bytes on startup
  //because of the way we write the next bit of data past the end of the last here, this is what limited the size of
  //the values. Oh, and ReadStringFromEEPROM only allows 31 character. TODO: Yike.
  debugln("Writing Config", "");
  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, 'G');

  EEPROM.write(16, config.dhcp);
  EEPROM.write(17, config.daylight);

  EEPROMWritelong(18, config.Update_Time_Via_NTP_Every); // 4 Byte

  EEPROMWritelong(22, config.timezone); // 4 Byte


  EEPROM.write(26, config.LED_R);
  EEPROM.write(27, config.LED_G);
  EEPROM.write(28, config.LED_B);

  EEPROM.write(32, config.IP[0]);
  EEPROM.write(33, config.IP[1]);
  EEPROM.write(34, config.IP[2]);
  EEPROM.write(35, config.IP[3]);

  EEPROM.write(36, config.Netmask[0]);
  EEPROM.write(37, config.Netmask[1]);
  EEPROM.write(38, config.Netmask[2]);
  EEPROM.write(39, config.Netmask[3]);

  EEPROM.write(40, config.Gateway[0]);
  EEPROM.write(41, config.Gateway[1]);
  EEPROM.write(42, config.Gateway[2]);
  EEPROM.write(43, config.Gateway[3]);


  WriteStringToEEPROM(64, config.ssid);
  WriteStringToEEPROM(96, config.password);
  WriteStringToEEPROM(128, config.ntpServerName);

  EEPROM.write(300, config.Logging);
  //EEPROM.write(300, config.AutoTurnOn);
  EEPROM.write(301, config.WakeCount);
  //EEPROM.write(301, config.AutoTurnOff);
  EEPROM.write(302, config.Connect);
  //EEPROM.write(302, config.TurnOnHour);
  EEPROMWritelong(303, config.Interval); // 4 Bytes
  //EEPROM.write(303, config.TurnOnMinute);
  //EEPROM.write(304, config.TurnOffHour);
  //EEPROM.write(305, config.TurnOffMinute);
  
  WriteStringToEEPROM(307, config.DeviceName);
  WriteStringToEEPROM(338, config.streamServerURL);

  EEPROM.commit();
}

boolean ReadConfig() {

  debug("Reading Configuration. ", "");
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' ) {
    debugln("Found!", "");
    config.dhcp = 	EEPROM.read(16);

    config.daylight = EEPROM.read(17);

    config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

    config.timezone = EEPROMReadlong(22); // 4 Byte

    config.LED_R = EEPROM.read(26);
    config.LED_G = EEPROM.read(27);
    config.LED_B = EEPROM.read(28);

    config.IP[0] = EEPROM.read(32);
    config.IP[1] = EEPROM.read(33);
    config.IP[2] = EEPROM.read(34);
    config.IP[3] = EEPROM.read(35);
    config.Netmask[0] = EEPROM.read(36);
    config.Netmask[1] = EEPROM.read(37);
    config.Netmask[2] = EEPROM.read(38);
    config.Netmask[3] = EEPROM.read(39);
    config.Gateway[0] = EEPROM.read(40);
    config.Gateway[1] = EEPROM.read(41);
    config.Gateway[2] = EEPROM.read(42);
    config.Gateway[3] = EEPROM.read(43);
    config.ssid = ReadStringFromEEPROM(64);
    config.password = ReadStringFromEEPROM(96);
    config.ntpServerName = ReadStringFromEEPROM(128);


    //config.AutoTurnOn = EEPROM.read(300);
    config.Logging = EEPROM.read(300);
    config.WakeCount = EEPROM.read(301);
    //config.AutoTurnOff = EEPROM.read(301);
    config.Connect = EEPROM.read(302);
    //config.TurnOnHour = EEPROM.read(302);
    config.Interval = EEPROMReadlong(303); // 4 Bytes
    //config.TurnOnMinute = EEPROM.read(303);
    //config.TurnOffHour = EEPROM.read(304);
    //config.TurnOffMinute = EEPROM.read(305);
    config.DeviceName = ReadStringFromEEPROM(307);
    config.streamServerURL = ReadStringFromEEPROM(338);
    return true;
  }
  else {
    // DEFAULT CONFIG
    config.ssid = "attwifi";
    config.password = "";
    config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.ntpServerName = "0.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  60;
    config.timezone = -10;
    config.daylight = true;
    config.DeviceName = "Not Named";
    //config.AutoTurnOn = false;
    config.Logging = false;
    config.WakeCount = 10;
    //config.AutoTurnOff = false;
    //config.TurnOnHour = 0;
    config.Connect = true;
    config.Interval = 0; //zero means stay on.
    //config.TurnOnMinute = 0;
    //config.TurnOffHour = 0;
    //config.TurnOffMinute = 0;
    config.streamServerURL = "http://www.massmind.org/techref/getline.asp?";
    WriteConfig();
    debug("","General config applied");
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
  WiFi.begin (config.ssid.c_str(), config.password.c_str());
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
    WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP);
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
int p1,p2=0;
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
        debug(" param:",response.substring(p1,p2));
        break;
      case ',':
      case '}':
        debug(", val:",response.substring(p2+1,i));
        if ( response.substring(p1,p2) == "interval" ) {
          config.Interval = response.substring(p2+1,i).toInt();
          if (config.Interval>0) config.sleepy=true; else config.sleepy=false;
          debugln(" interval=",config.Interval);
          }
        p1 = i+1; //start of next parameter if it was ','
        break;
      }
    }
  if (0==p1) {text = response;} //if no JASON, entire string is text for device.
  return text;
  }

#endif

