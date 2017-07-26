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
#include <JsonParser.h>           // https://github.com/henrikekblad/ArduinoJson
#include <JsonGenerator.h>        // https://github.com/henrikekblad/ArduinoJson
#include "overrides.h"            // https://gist.github.com/ClemRz/88b5ba77cd73a0035fd2fc6a843acb55
#include "structures.h"           // https://github.com/ClemRz/Introduction-to-IoT#use-structures

using namespace ArduinoJson;

// General
#define MICROSEC                  1000000L
#define MILLISEC                  1000L
#define SEC                       1L
#define MINUTE                    (unsigned int) 60L*SEC
#define HOUR                      (unsigned int) 60L*MINUTE
#define DAY                       (unsigned long) 24L*HOUR

/* 
 * ======================================
 *      User defined constants
 * ======================================
*/
#define DEBUG                     1
#define AP_SSID                   "WiFibonacciClk"
#define AP_PASSWORD               "fibonacci"

// Pins
#define SDA                       4               // I2C data
#define SCL                       5               // I2C clock
#define MODE_BUTTON               12
#define PALETTE_BUTTON            13
#define LED_DATA                  14               // Pin connected to strip's data wire
#define BRIGHTNESS_BUTTON         16

// Serial
#define DS3231_I2C_ADDRESS        0x68            // RTC I2C address

// Buttons
#define BUTTONS_SIZE              17              // Max button pin + 1
#define DEBOUNCE_DELAY_MS         10L

// Clock
#define FADING_DELAY_MS           8               // Delay between each steps
#define FADING_STEP               1               // 214 steps total means a delay of 214/1*8 = 1,712ms to fade the complete range aprox.
#define CLOCK_PIXELS              5               // Number of fisical cells
#define LEDS_SIZE                 9               // Number of LEDs
#define MODES_SIZE                5               // Number of modes
#define CLOCK_MODE                0
#define RAINBOW_CYCLE_MODE        1
#define RAINBOW_MODE              2
#define PULSE_MODE                3
#define FLASH_LIGHT_MODE          4

// File system configs
#define PALETTES_PATH             "/palettes"
#define SETTINGS_FILE_PATH        "/settings.json"
#define PALETTE_TYPE              1
#define SETTINGS_TYPE             2

// Defaults
#define DEFAULT_PALETTE           "[\"ffffff\",\"ff0a0a\",\"0aff0a\",\"0a0aff\"]"
#define DEFAULT_SETTINGS          "{\"mode\":0,\"flashLightColor\":\"ffffff\",\"rainbowDelay\":20,\"pulse\":{\"color\":\"ffb330\",\"delay\":20}}"

// HTML
#define UI_HTTP_PAYLOAD           "HTTP/1.1 200 OK\r\nServer: esp8266\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><meta name=viewport content=\"initial-scale=1,maximum-scale=1,user-scalable=no\"><style>html,body{font-family:Arial;font-size:14px;background:#fff;padding:3px;color:#000;margin:0;width:100%;line-height:2em}</style></head><body><form> Mode: <select name=m onchange=\"z(this,'m')\"><option value=0>Clock</option><option value=1>Rainbow Cycle</option><option value=2>Rainbow</option><option value=3>Pulse</option><option value=4>Constant Light</option></select><br> Date and time: <input type=datetime-local name=t oninput=\"z(this,'t')\"><input type=button onclick=\"x(t)\" value=now><br> FlashLight color: <input name=f type=color oninput=\"z(this,'f')\"><br>Pulse color: <input name=p type=color oninput=\"z(this,'p')\"><br> Pulse delay (ms): <input name=d type=range min=1 max=100 oninput=\"y(this,e,'p')\"><input type=number name=e min=1 max=100 oninput=\"y(this,d,'p')\"><br> Rainbow delay (ms): <input name=r type=range min=1 max=100 oninput=\"y(this,s,'r')\"><input type=number name=s min=1 max=100 oninput=\"y(this,r,'r')\"></form></body><script>function v(a){return(a<100?\"0\":\"\")+a}function w(a){return(a<10?\"0\":\"\")+a}function x(j){var i=new Date(),h=w(i.getHours()),o=w(i.getMinutes()),n=w(i.getSeconds()),m=i.getFullYear(),l=i.getMonth(),k=w(i.getDate());j.value=m+\"-\"+w(l+1)+\"-\"+k+\"T\"+h+\":\"+o+\":\"+n;z({value:[\"Jan\",\"Feb\",\"Mar\",\"Apr\",\"May\",\"Jun\",\"Jul\",\"Aug\",\"Sep\",\"Oct\",\"Nov\",\"Dec\"][l]+\" \"+k+\" \"+m+h+\":\"+o+\":\"+n},\"t\")}function y(e,d,f){d.value=e.value;z({value:v(w(e.value))},f)}function z(d,c){_c.send(c+d.value)}var _c=new WebSocket(\"ws://192.168.4.1:81/\",[\"arduino\"]);_c.onmessage=function(c){var a=JSON.parse(c.data),b=document.querySelector(\"form\");b.m.value=a.mode;b.t.value=a.dateTime;b.f.value=\"#\"+a.flashLightColor;b.p.value=\"#\"+a.pulse.color;b.d.value=a.pulse.delay;b.e.value=b.d.value;b.r.value=a.rainbowDelay;b.s.value=b.r.value};</script></html>"

// Global variables
Settings _settings;
std::vector<Palette> _palettesV;
unsigned long _lastDebounceTime = 0;
uint8_t
  _brightnessBackup = 255,
  _brightness = 255; //TODO check in settings
int
  _paletteIndex = 0, //TODO check in settings
  _j = 0;
bool
  _lastButtonState[BUTTONS_SIZE],
  _currentButtonState[BUTTONS_SIZE],
  _refreshLedStrip = true,
  _ledStripOn = true;
byte
  _bits[CLOCK_PIXELS],
  _hours = 0,
  _minutes = 0;
RtcDS3231<TwoWire> _clock(Wire);
Adafruit_NeoPixel _ledStrip = Adafruit_NeoPixel(LEDS_SIZE, LED_DATA, NEO_RGB + NEO_KHZ800);
MyWebSocketsServer _webSocket = MyWebSocketsServer(81, UI_HTTP_PAYLOAD);

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

