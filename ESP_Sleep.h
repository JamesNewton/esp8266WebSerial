#ifndef ESP_SLEEP_H
#define ESP_SLEEP_H

#define RTCMEMORYSTART 65
#define RTCMEMORYLEN 127

#define MICROSECONDS 1000000

/*
http://www.esp8266.com/wiki/doku.php?id=esp8266_power_usage
https://github.com/esp8266/Arduino/blob/master/doc/libraries.md#esp-specific-apis

ESP.deepSleep(uint32t SLEEPTIME_MICROSECONDS);
ESP.deepSleep(uint32t SLEEPTIME_MICROSECONDS, /mode/);

Note this will wake up in the setup routine, NOT in the loop!

Mode               Description
0 WAKE_RF_DEFAULT  Radio calibration after deep-sleep wake up depends on init data byte 108. 
1 WAKE_RFCAL       Radio calibration is done after deep-sleep wake up; this increases the current consumption.
2 WAKE_NO_RFCAL    No radio calibration after deep-sleep wake up; this reduces the current consumption.
4 WAKE_RF_DISABLED Disable RF after deep-sleep wake up, just like modem sleep; this has the least current consumption; 
                       the device is not able to transmit or receive data after wake up.


*/

/*
rst_info *reset;
reset = ESP.getResetInfoPtr();
select reset->reason
Reason
0  REASON_DEFAULT_RST       normal startup by power on
1  REASON_WDT_RST           hardware watch dog reset
2  REASON_EXCEPTION_RST     exception reset, GPIO status won’t change
3  REASON_SOFT_WDT_RST      software watch dog reset, GPIO status won’t change 
4  REASON_SOFT_RESTART      software restart ,system_restart , GPIO status won’t change
5  REASON_DEEP_SLEEP_AWAKE  wake up from deep-sleep via RTC
6  REASON_EXT_SYS_RST       external system reset 
 */

/*

https://github.com/esp8266/Arduino/blob/master/doc/libraries.md#esp-specific-apis
https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/RTCUserMemory/RTCUserMemory.ino

ESP.rtcUserMemoryWrite(offset, &data, sizeof(data)) 
ESP.rtcUserMemoryRead(offset, &data, sizeof(data)) 
allow data to be stored in and retrieved from the RTC user memory of the chip respectively. 
Total size of RTC user memory is 512 bytes, so offset + sizeof(data) shouldn't exceed 512. 
Data should be 4-byte aligned. The stored data can be retained between deep sleep cycles. 
However, the data might be lost after power cycling the chip. data must be cast to (uint32_t*)

struct {
  float count;
  int other;
  //...whatever you want in here.
} rtcmem;
static_assert (sizeof(rtcmem) < 512, "RTC can't hold more than 512 bytes");

//in setup, if reset->reason == REASON_DEEP_SLEEP_AWAKE
ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcmem, sizeof(rtcmem));

//to save and go to sleep
ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcmem, sizeof(rtcmem)); //write data to RTC memory so we don't loose it.
ESP.deepSleep(MICROSECONDS, WAKE_NO_RFCAL); //deep sleep, assume RF ok, wake back up in setup.

 */


#endif ESP_SLEEP_H

