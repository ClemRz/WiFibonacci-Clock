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

/*TODOs:
    - only first palette is read from SPIFFS
    - save the settings every once in a while
    - add a reset factory settings button in the UI
    - add a save / restore UI for the settings
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
#define MODES_SIZE                6               // Number of modes
#define CLOCK_MODE                0
#define RAINBOW_CYCLE_MODE        1
#define RAINBOW_MODE              2
#define RANDOM_MODE               3
#define PULSE_MODE                4
#define FLASH_LIGHT_MODE          5

// File system configs
#define PALETTES_PATH             "/palettes"
#define SETTINGS_FILE_PATH        "/settings.json"

// Defaults
#define DEFAULT_PALETTE           "[\"ffffff\",\"ff0a0a\",\"0aff0a\",\"0a0aff\"]"
#define DEFAULT_SETTINGS          "{\"mode\":0,\"brightness\":255,\"palette\":0,\"flashLightColor\":\"ffffff\",\"rainbowDelay\":40,\"pulse\":{\"color\":\"ffb330\",\"delay\":20},\"random\":{\"ease\":25,\"delay\":1750}}"

// HTML
/*
* Can be found in ./generated/
* Use grunt to generate those from client.html (install dependencies with npm)
*/
#define UI_HTTP_PAYLOAD           "HTTP/1.1 200 OK\r\nServer: esp8266\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><head><meta name=viewport content=\"initial-scale=1,maximum-scale=1,user-scalable=no\"></head><body><script>function s(a){return document.querySelector(a)}function t(a,b){var c=document.createElement(\"div\");c.innerHTML=b;while(c.children.length>0){a.appendChild(c.children[0])}}var _a,_c,_f,_w=new WebSocket(\"ws://192.168.4.1:81/\",[\"arduino\"]);_w.onmessage=function(e){var data=e.data;if(_c){if(data==\"o\"){_c=JSON.parse(_c);_c.push(_a.split(\".\")[0]);o(_c);_f.i.value=\"\"}else{p()}_c=0}switch(data[0]){case\"{\":case\"[\":var json=JSON.parse(data);if(typeof json.mode!=\"undefined\"){_f.b.value=json.brightness;_f.c.value=_f.b.value;_f.d.value=json.pulse.delay;_f.e.value=_f.d.value;_f.f.value=\"#\"+json.flashLightColor;_f.g.value=json.random.ease;_f.h.value=_f.g.value;_f.j.value=json.random.delay;_f.k.value=_f.j.value;m(json.palette);_f.m.value=json.mode;_f.p.value=\"#\"+json.pulse.color;_f.r.value=json.rainbowDelay;_f.s.value=_f.r.value;_f.t.value=json.dateTime}else{o(json)}break;case\"<\":t(s(data.indexOf(\"<style>\")==0?\"head\":\"body\"),data);_f=s(\"form\");break;default:eval(data)}};</script></body></html>"
#define UI_JS_SCRIPT              "window.m=function(n){(_f.l[n]?_f.l[n]:_f.l).checked=1},window.n=function(n){for(var e=n.parentNode,o=0;e.previousElementSibling;)e=e.previousElementSibling,o++;return v(o)},window.o=function(n){for(var e=Math.random(),o='<div><input type=\"radio\" name=\"l\" id=\"l'+e+'\" onclick=\"zl(this)\"><label for=\"l'+e+'\">',i=0;i<4;i++)o+='<span style=\"background-color:#'+n[i]+'\"></span>';o+=n[4]+\"</label>\",_f.l&&(o+='<a href=\"#\" onclick=\"zd(this)\">X</a>'),o+=\"</div>\",t(s(\"#l\"),o)},window.p=function(){alert(\"Failure\")},window.q=function(n){var e=n.files[0],o=new FileReader;o.onerror=p,o.onload=function(n){_c=n.target.result,_a=e.name,zz(_a,\"a\"),zz(_c,\"c\")},o.readAsText(e)},window.u=function(n){var e=1*n.value;return\"\"===e||n.min&&e<1*n.min||n.max&&e>1*n.max},window.v=function(n){return w(n,2)},window.w=function(n,e){for(n+=\"\";n.length<e;)n=\"0\"+n;return n},window.x=function(n){var e=new Date,o=v(e.getHours()),i=v(e.getMinutes()),t=v(e.getSeconds()),a=e.getFullYear(),r=e.getMonth(),l=v(e.getDate());n.value=a+\"-\"+v(r+1)+\"-\"+l+\"T\"+o+\":\"+i+\":\"+t,zz([\"Jan\",\"Feb\",\"Mar\",\"Apr\",\"May\",\"Jun\",\"Jul\",\"Aug\",\"Sep\",\"Oct\",\"Nov\",\"Dec\"][r]+\" \"+l+\" \"+a+o+\":\"+i+\":\"+t,\"t\")},window.y=function(n,e,o){if(!u(n)){var i=n.value;e.value=i,zz(w(i,4),o)}},window.z=function(n,e){if(!u(n)){var o=n.value;_w.send(e+o)}},window.zz=function(n,e){z({value:n},e)},window.zl=function(e){zz(n(e),\"l\")},window.zd=function(e){zz(n(e),\"d\"),e.parentNode.remove(),m(0)},window.zm=function(n){z(n,\"m\")};"
#define UI_HTML_BODY              "<header><h1>WiFibonacci Clock Settings</h1></header><section><form><fieldset><legend>General</legend><input type=radio name=m id=m0 onclick=\"zm(this)\" value=0><label for=m0>Clock</label><br><input type=radio name=m id=m1 onclick=\"zm(this)\" value=1><label for=m1>Rainbow Cycle</label><br><input type=radio name=m id=m2 onclick=\"zm(this)\" value=2><label for=m2>Rainbow</label><br><input type=radio name=m id=m3 onclick=\"zm(this)\" value=3><label for=m3>Random</label><br><input type=radio name=m id=m4 onclick=\"zm(this)\" value=4><label for=m4>Pulse</label><br><input type=radio name=m id=m5 onclick=\"zm(this)\" value=5><label for=m5>Constant Light</label><br> Brightness: <input name=b type=range min=1 max=255 oninput=\"y(this,c,'b')\"><input type=number name=c min=1 max=255 oninput=\"y(this,b,'b')\"></fieldset><fieldset><legend>Clock</legend> Date and time:<br><input type=datetime-local name=t oninput=\"z(this,'t')\"><input type=button onclick=\"x(t)\" value=now><br> Palettes:<br><div class=h><span>off</span><span>hours</span><span>minutes</span><span>both</span></div><div id=l></div> Upload palette:<br><input type=file name=i><input type=button onclick=\"q(i)\" value=upload></fieldset><fieldset><legend>Rainbow</legend> Delay (ms): <input name=r type=range min=1 max=100 oninput=\"y(this,s,'r')\"><input type=number name=s min=1 max=100 oninput=\"y(this,r,'r')\"></fieldset><fieldset><legend>Random</legend> Ease (ms): <input name=g type=range min=1 max=100 oninput=\"y(this,h,'g')\"><input type=number name=h min=1 max=100 oninput=\"y(this,g,'g')\"><br> Delay (ms): <input name=j type=range min=1 max=9999 oninput=\"y(this,k,'j')\"><input type=number name=k min=1 max=9999 oninput=\"y(this,j,'j')\"></fieldset><fieldset><legend>Pulse</legend> Color: <input name=p type=color oninput=\"z(this,'p')\"><br> Delay (ms): <input name=d type=range min=1 max=100 oninput=\"y(this,e,'p')\"><input type=number name=e min=1 max=100 oninput=\"y(this,d,'p')\"></fieldset><fieldset><legend>Constant light</legend> Color: <input name=f type=color oninput=\"z(this,'f')\"></fieldset></form></section><footer> &copy; 2017 Clément Ronzon </footer>"
#define UI_HTML_STYLE             "<style>html,body{font-family:Arial;font-size:14px;background:#fff;padding:3px;color:#000;margin:0;width:100%;line-height:2em;box-sizing:border-box}section{width:350px;margin:0 auto}fieldset>legend{font-weight:bolder}label>span,div>span{display:inline-block;margin-right:2px}div.h{margin-left:22px}div>span{width:52px;text-align:center}label>span{width:50px;height:25px;border:1px solid black;vertical-align:bottom}header,footer{text-align:center}footer{color:#888;font-size:.75rem}#l a{margin-left:5px}</style>"

// Global variables
Settings _settings;
std::vector<Palette> _palettesV;
unsigned long
  _lastDebounceTime = 0,
  _timing = 0,
  _timer1 = 0,
  _timer2 = 0;
uint8_t
  _brightnessBackup = 255,
  _randomBrightness[CLOCK_PIXELS];
uint32_t _randomColor[CLOCK_PIXELS];
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
  if (shouldRefreshLedStrip()) handleModes();
  _webSocket.loop();
}

