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
#define BRIGHTNESS_BUTTON         13
#define LED_DATA                  14               // Pin connected to strip's data wire

// Serial
#define DS3231_I2C_ADDRESS        0x68            // RTC I2C address

// Buttons
#define BUTTONS_SIZE              15              // Max button pin + 1
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

// Defaults
#define DEFAULT_PALETTE           "[\"ffffff\",\"ff0a0a\",\"0aff0a\",\"0a0aff\"]"
#define DEFAULT_SETTINGS          "{\"mode\":0,\"brightness\":255,\"palette\":0,\"flashLightColor\":\"ffffff\",\"rainbowDelay\":20,\"pulse\":{\"color\":\"ffb330\",\"delay\":20}}"

// HTML
/*
* Can be found in ./generated/
* Use grunt to generate those from client.html (install dependencies with npm)
*/
#define UI_HTTP_PAYLOAD           "HTTP/1.1 200 OK\r\nServer: esp8266\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><meta name=viewport content=\"initial-scale=1,maximum-scale=1,user-scalable=no\"></head><body></body><script>function p(){alert(\"Failure\")}function q(d){var e=d.files[0],c=new FileReader();c.onerror=p;c.onload=function(a){_e=1;zz(e.name,\"a\");zz(a.target.result,\"c\")};c.readAsText(e)}function s(b){return document.querySelector(b)}function t(b,a){var c=document.createElement(\"div\");c.innerHTML=a;while(c.children.length>0){b.appendChild(c.children[0])}}function u(b){var c=b.value*1;return c===\"\"||b.min&&c<b.min*1||b.max&&c>b.max*1}function v(a){return(a<100?\"0\":\"\")+a}function w(a){return(a<10?\"0\":\"\")+a}function x(j){var i=new Date(),h=w(i.getHours()),o=w(i.getMinutes()),n=w(i.getSeconds()),m=i.getFullYear(),l=i.getMonth(),k=w(i.getDate());j.value=m+\"-\"+w(l+1)+\"-\"+k+\"T\"+h+\":\"+o+\":\"+n;zz([\"Jan\",\"Feb\",\"Mar\",\"Apr\",\"May\",\"Jun\",\"Jul\",\"Aug\",\"Sep\",\"Oct\",\"Nov\",\"Dec\"][l]+\" \"+k+\" \"+m+h+\":\"+o+\":\"+n,\"t\")}function y(f,e,h){if(u(f)){return}var g=f.value;e.value=g;zz(v(w(g)),h)}function z(e,d){if(u(e)){return}var f=e.value;_c.send(d+f)}function zz(d,c){z({value:d},c)}function zl(b){zz(w(b.value),\"l\")}function zm(b){z(b,\"m\")}var _c=new WebSocket(\"ws://192.168.4.1:81/\",[\"arduino\"]),_e,_i=0;_c.onmessage=function(k){var l=k.data;if(_e){if(l==\"o\"){s(\"#l\").innerHTML=\"\"}else{p()}_e=0}if(l[0]==\"{\"||l[0]==\"[\"){var a=JSON.parse(l),g=s(\"form\");if(typeof a.mode!=\"undefined\"){g.m.value=a.mode;g.b.value=a.brightness;g.c.value=g.b.value;g.t.value=a.dateTime;g.f.value=\"#\"+a.flashLightColor;g.p.value=\"#\"+a.pulse.color;g.d.value=a.pulse.delay;g.e.value=g.d.value;g.r.value=a.rainbowDelay;g.s.value=g.r.value;g.l.value=a.palette}else{var c='<input type=\"radio\" name=\"l\" id=\"l'+_i+'\" onclick=\"zl(this)\" value=\"'+_i+'\"><label for=\"l'+_i+'\">';for(var b=0;b<4;b++){c+='<span style=\"background-color:#'+a[b]+'\"></span>'}c+=a[4]+\"</label><br>\";t(s(\"#l\"),c);if(!_i){g.l.checked=1}_i++}}else{t(s(l.indexOf(\"<style>\")==0?\"head\":\"body\"),l)}};</script></html>"
#define UI_HTML_BODY              "<header><h1>WiFibonacci Clock Settings</h1></header><section><form><fieldset><legend>General</legend><input type=radio name=m id=m0 onclick=\"zm(this)\" value=0><label for=m0>Clock</label><br><input type=radio name=m id=m1 onclick=\"zm(this)\" value=1><label for=m1>Rainbow Cycle</label><br><input type=radio name=m id=m2 onclick=\"zm(this)\" value=2><label for=m2>Rainbow</label><br><input type=radio name=m id=m3 onclick=\"zm(this)\" value=3><label for=m3>Pulse</label><br><input type=radio name=m id=m4 onclick=\"zm(this)\" value=4><label for=m4>Constant Light</label><br> Brightness: <input name=b type=range min=1 max=255 oninput=\"y(this,c,'b')\"><input type=number name=c min=1 max=255 oninput=\"y(this,b,'b')\"></fieldset><fieldset><legend>Clock</legend> Date and time: <input type=datetime-local name=t oninput=\"z(this,'t')\"><input type=button onclick=\"x(t)\" value=now><br> Palettes:<br><div class=h><span>off</span><span>hours</span><span>minutes</span><span>both</span></div><div id=l></div> Upload palette:<br><input type=file name=i><input type=button onclick=\"q(i)\" value=upload></fieldset><fieldset><legend>Rainbow</legend> Delay (ms): <input name=r type=range min=1 max=100 oninput=\"y(this,s,'r')\"><input type=number name=s min=1 max=100 oninput=\"y(this,r,'r')\"></fieldset><fieldset><legend>Pulse</legend> Color: <input name=p type=color oninput=\"z(this,'p')\"><br> Delay (ms): <input name=d type=range min=1 max=100 oninput=\"y(this,e,'p')\"><input type=number name=e min=1 max=100 oninput=\"y(this,d,'p')\"></fieldset><fieldset><legend>Constant light</legend> Color: <input name=f type=color oninput=\"z(this,'f')\"></fieldset></form></section><footer> &copy; 2017 Clément Ronzon </footer>"
#define UI_HTML_STYLE             "<style>html,body{font-family:Arial;font-size:14px;background:#fff;padding:3px;color:#000;margin:0;width:100%;line-height:2em;box-sizing:border-box}section{width:393px;margin:0 auto}fieldset>legend{font-weight:bolder}label>span,div>span{display:inline-block;margin-right:2px}div.h{margin-left:22px}div>span{width:52px;text-align:center}label>span{width:50px;height:25px;border:1px solid black;vertical-align:bottom}header,footer{text-align:center}footer{color:#888;font-size:.75rem}</style>"

// Global variables
Settings _settings;
std::vector<Palette> _palettesV;
unsigned long _lastDebounceTime = 0;
uint8_t
  _brightnessBackup = 255;
int _j = 0;
bool
  _lastButtonState[BUTTONS_SIZE],
  _currentButtonState[BUTTONS_SIZE],
  _refreshLedStrip = true,
  _ledStripOn = true;
byte
  _bits[CLOCK_PIXELS],
  _hours = 0,
  _minutes = 0;
String _receivedFileName;
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

