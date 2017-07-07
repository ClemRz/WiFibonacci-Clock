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
#define AP_SSID               "WiFibonacciClk"
#define AP_PASSWORD           "fibonacci"

/* 
 * ======================================
 *      User defined settings
 * ======================================
*/
#define RAINBOW_DELAY_MS      20
//const uint32_t _white = _ledStrip.Color(255, 255, 255);
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
#define BUTTONS_SIZE          17              // Max button pin + 1
#define DEBOUNCE_DELAY_MS     10L

// Clock
#define MODES_SIZE            4
#define LEDS_SIZE             9               // Number of LEDs
#define CLOCK_PIXELS          5               // Number of fisical cells
#define FADING_DELAY_MS       8               // Delay between each steps
#define FADING_STEP           1               // 214 steps total means a delay of 214/1*8 = 1,712ms to fade the complete range aprox.

// File system configs
#define PALETTES_PATH         "/palettes"
#define SETTINGS_FILE_PATH    "/settings.json"
#define PALETTE_TYPE          1
#define SETTINGS_TYPE         2

// Defaults
#define DEFAULT_PALETTE       "[[255, 255, 255],[255, 10, 10],[10, 255, 10],[10, 10, 255]]"
#define DEFAULT_SETTINGS      ""

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

