# esp8266WebSerial
Put anything with a TTL serial connection on the local network with browser access or log to a server... for under 10 bucks

- ESPAsyncWebServer for reliable operation, local config in browser, direct web based serial "terminal", etc...
- File system support to upload custom default web page, libraries, css, etc... via "hidden" page to avoid end user confusion. 
This allows customization and rebranding for your solution without the need for Arduino code development. Put your own html/javascript in the device for user interface.
- Log incoming data / status to server, stream commands from server out to the connected device
- Low power cycle config: sleep for x seconds, wake send data / status to server as available or every x wake cycles.
- "Blink Detect" input to watch error lights, etc... on remote devices (with optional photodiode and flipflop for detection during sleep)
- Wake without radio for very low power input monitoring
- "Pico Jason" interpreter for commands from server to ESP to configure power cycles, logging.
- Support for ST7735 and ILI9341 and other LCD displays via [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI#tft_espi) _in overlap mode only_ for MAC, SSID / IP address display and 
device data extraction / display via scanf codes, slope, and offset.
- Device uses alternate TX/RX pins so power up bootloader messages are NOT sent to device and debug messages are available on Serial Monitor. 
- Basic X-ON/X-OFF support to avoid buffer overruns on connected device (TODO: CTS monitoring)
- Configurable device port enable / disable to avoid powering level converters or triggering device connect power up.
- Configurable device baud rate, power on initialization message, and data extraction triggers.
- Optional support for "NeoPixel" LEDs (commented out by default).

Notes:


Due to a [bug / limitation in ESPAsyncWebServer's template system](https://github.com/me-no-dev/ESPAsyncWebServer/issues/333#issuecomment-370595466), and the fact that % is often used in the device configuration, you have to edit the /src/WebResponseImpl.h file in that library to change:
````
  #define TEMPLATE_PLACEHOLDER '%'
````
to
````
#define TEMPLATE_PLACEHOLDER '`'
````

P.S. Don't even try to get this to work with an ESP-8266 module that wasn't made by AI-THINKER. [The knock offs will NOT work](https://github.com/me-no-dev/ESPAsyncWebServer/issues/374).

Due to a [bug / limitation in ESPAsyncWebServer's template system](https://github.com/me-no-dev/ESPAsyncWebServer/issues/333#issuecomment-370595466), and the fact that % is often used in the device configuration, you have to edit the /src/WebResponseImpl.h file in that library to change:
````
  #define TEMPLATE_PLACEHOLDER '%'
````
to
````
#define TEMPLATE_PLACEHOLDER '`'
````

P.S. Don't even try to get this to work with an ESP-8266 module that wasn't made by AI-THINKER. [The knock offs will NOT work](https://github.com/me-no-dev/ESPAsyncWebServer/issues/374).

See:
http://techref.massmind.org/techref/ESP8266/WebSerial.htm for documentation, screenshots, and examples
http://techref.massmind.org/techref/esp-8266.htm For notes on the esp-8266, how to connect and program it from the Arduino IDE.
