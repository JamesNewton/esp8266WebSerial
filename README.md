# esp8266WebSerial
Put anything with a TTL serial connection on the local network with browser access or log to a server... for under 10 bucks

- ESPAsyncWebServer for reliable operation
- Support for ST7735 and ILI9341 and other LCD displays via TFT_eSPI
- Log incoming data / status to server, stream commands from server out to the connected device
- Low power cycle config: sleep for x seconds, wake send data / status to server as available or every x wake cycles.
- "Blink Detect" input to watch error lights, etc... on remote devices (with optional photodiode and flipflop for detection during sleep)
- Wake without radio for very low power input monitoring
- "Pico Jason" interpreter for commands from server to ESP to configure power cycles, logging.

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
