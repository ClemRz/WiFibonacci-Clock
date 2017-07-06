/*
    Copyright (C) 2017 Clément Ronzon

    This file is part of WiFibonacciClock.

    WiFibonacciClock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WiFibonacciClock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WiFibonacciClock.  If not, see <http://www.gnu.org/licenses/>.

    Source code inspired from Fibonacci Clock https://github.com/pchretien/fibo
 */

/*#include <StandardCplusplus.h>    // https://github.com/maniacbug/StandardCplusplus
#include <serstream>              // https://github.com/maniacbug/StandardCplusplus
#include <vector>                 // https://github.com/maniacbug/StandardCplusplus
#include <iterator>               // https://github.com/maniacbug/StandardCplusplus */
#include <Wire.h>                 // https://github.com/esp8266/Arduino/
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino/
#include <WiFiClient.h>           // https://github.com/esp8266/Arduino/
#include <Hash.h>                 // https://github.com/esp8266/Arduino/
#include <algorithm>              // https://github.com/esp8266/Arduino/
#include "FS.h"                   // https://github.com/esp8266/Arduino/
#include <RtcDS3231.h>            // https://github.com/Makuna/Rtc
#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <WebSocketsServer.h>     // https://github.com/Links2004/arduinoWebSockets
//#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include <JsonParser.h>           // https://github.com/henrikekblad/ArduinoJson
#include <JsonGenerator.h>        // https://github.com/henrikekblad/ArduinoJson
#include "overrides.h"            // https://gist.github.com/ClemRz/88b5ba77cd73a0035fd2fc6a843acb55
#include "structures.h"           // https://github.com/ClemRz/Introduction-to-IoT#use-structures

using namespace ArduinoJson;

// General
#define MICROSEC              1000000L
#define MILLISEC              1000L
#define SEC                   1L
#define MINUTE                (unsigned int) 60L*SEC
#define HOUR                  (unsigned int) 60L*MINUTE
#define DAY                   (unsigned long) 24L*HOUR

/* 
 * ======================================
 *      User defined constants
 * ======================================
*/
#define DEBUG                 1
#define FADING_STEP           3
#define AP_SSID               "WiFibonacciClk"
#define AP_PASSWORD           "fibonacci"
// ======================================

// Pins
#define SDA                   4               // I2C data
#define SCL                   5               // I2C clock
#define MODE_BUTTON           12
#define PALETTE_BUTTON        13
#define BRIGHTNESS_BUTTON     16
#define LED_DATA              14               // Pin connected to strip's data wire

// Serial
#define DS3231_I2C_ADDRESS    0x68            // RTC I2C address

// Buttons
#define BUTTONS_SIZE          9               // Max button pin + 1
#define DEBOUNCE_DELAY_MS     10L

// Clock
#define MODES_SIZE            4
#define LEDS_SIZE             9               // Number of LEDs
#define CLOCK_PIXELS          5               // Number of fisical cells
#define RAINBOW_DELAY_MS      20

// File system configs
#define PALETTES_PATH         "/palettes/"
#define SETTINGS_FILE_PATH    "/settings.json"

// Defaults
#define DEFAULT_PALETTES      "[{\"off\":{\"r\":255,\"g\":255,\"b\":255},\"hours\":{\"r\":255,\"g\":10,\"b\":10},\"minutes\":{\"r\":10,\"g\":255,\"b\":10},\"both\":{\"r\":10,\"g\":10,\"b\":255}}]"
#define DEFAULT_SETTINGS      ""

// Gamma Correction (See https://learn.adafruit.com/led-tricks-gamma-correction)
const uint8_t PROGMEM GAMMA_8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255};

// Global variables
Settings _settings;
std::vector<Palette> _palettesV;
unsigned long _lastDebounceTime = 0;
uint8_t _brightness = 255;
int
  _paletteIndex = 0,
  _modeIndex = 0,
  _j = 0;
bool
  _lastButtonState[BUTTONS_SIZE],
  _currentButtonState[BUTTONS_SIZE],
  _refreshLedStrip = false,
  _ledStripOn = true;
byte
  _bits[CLOCK_PIXELS],
  _hours = 0,
  _minutes = 0;
RtcDS3231<TwoWire> _clock(Wire);
Adafruit_NeoPixel _ledStrip = Adafruit_NeoPixel(LEDS_SIZE, LED_DATA, NEO_RGB + NEO_KHZ800);
MyWebSocketsServer _webSocket = MyWebSocketsServer(81);

// Global constants
const uint32_t _white = _ledStrip.Color(255, 255, 255);

void setup(void) {
#if DEBUG
  initSerial();
#endif
  initFS();
  initButtons();
  initDS3231();
  initLedStrip();
  initRandom();
  initAP();
  initWebSocket();
}

void loop(void) {
  handleButtons();
  handleModes();
  _webSocket.loop();
}

