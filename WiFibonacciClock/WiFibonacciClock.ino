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
#include <ESP8266WebServer.h>     // https://github.com/esp8266/Arduino/
#include <Hash.h>                 // https://github.com/esp8266/Arduino/
#include <algorithm>              // https://github.com/esp8266/Arduino/
#include "FS.h"                   // https://github.com/esp8266/Arduino/
#include <RtcDS3231.h>            // https://github.com/Makuna/Rtc
#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <WebSocketsServer.h>     // https://github.com/Links2004/arduinoWebSockets
#include <JsonParser.h>           // https://github.com/henrikekblad/ArduinoJson
#include <JsonGenerator.h>        // https://github.com/henrikekblad/ArduinoJson
#include "structures.h"           // https://github.com/ClemRz/Introduction-to-IoT#use-structures

using namespace ArduinoJson;

// General
#define MICROSEC                  1000000L
#define MILLISEC                  1000L
#define SEC                       1L
#define MINUTE                    (unsigned int) 60L*SEC
#define HOUR                      (unsigned int) 60L*MINUTE
#define DAY                       (unsigned long) 24L*HOUR

// Debug
#define DEBUG                     0               // Debug current sketch


// Wifi
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
#define UI_HTTP_PAYLOAD           "<!DOCTYPE html><html><head><meta name=viewport content=\"initial-scale=1,maximum-scale=1,user-scalable=no\"></head><body><script>function s(a){return document.querySelector(a)}function sa(a){return document.querySelectorAll(a)}function t(a,b){var c=document.createElement(\"div\");c.innerHTML=b;while(c.children.length>0){a.appendChild(c.children[0])}}var _f,_w=new WebSocket(\"ws://\"+location.hostname+\":81/\",[\"arduino\"]);_w.onmessage=function(messageEvent){var data=messageEvent.data;if(data==\"e\"){p()}else{switch(data[0]){case\"{\":o(JSON.parse(data));break;case\"[\":oo(JSON.parse(data));break;case\"<\":t(s(data.indexOf(\"<style>\")==0?\"head\":\"body\"),data);_f=s(\"form\");break;case\"d\":var i=data.slice(1)*1+1;s(\"#l div:nth-child(\"+i+\")\").remove();break;default:eval(data)}}};_w.onerror=function(a){};_w.onclose=function(a){alert(\"Wifi signal interrupted\");s(\"body\").className=\"disabled\";sa(\"body input\").forEach(function(b){b.disabled=1})};</script></body></html>"
#define UI_JS_SCRIPT              "window.dl=function(){data={mode:1*_f.m.value,brightness:1*_f.b.value,palette:1*n(sa(\"input[name=l]:checked\")[0]),flashLightColor:_f.f.value.replace(/#/,\"\"),rainbowDelay:1*_f.r.value,pulse:{color:_f.p.value.replace(/#/,\"\"),delay:1*_f.d.value},random:{ease:1*_f.g.value,delay:1*_f.j.value}};var e=document.createElement(\"a\");e.setAttribute(\"href\",\"data:application/json;charset=utf-8,\"+encodeURIComponent(JSON.stringify(data))),e.setAttribute(\"download\",\"settings.json\"),e.style.display=\"none\",document.body.appendChild(e),e.click(),document.body.removeChild(e)},window.m=function(e){(_f.l[e]?_f.l[e]:_f.l).checked=1},window.n=function(e){for(var n=e.parentNode,a=0;n.previousElementSibling;)n=n.previousElementSibling,a++;return v(a)},window.o=function(e){_f.b.value=e.brightness,_f.c.value=_f.b.value,_f.d.value=e.pulse.delay,_f.e.value=_f.d.value,_f.f.value=\"#\"+e.flashLightColor,_f.g.value=e.random.ease,_f.h.value=_f.g.value,_f.j.value=e.random.delay,_f.k.value=_f.j.value,m(e.palette),_f.m.value=e.mode,_f.p.value=\"#\"+e.pulse.color,_f.r.value=e.rainbowDelay,_f.s.value=_f.r.value,_f.t.value=e.dateTime},window.oo=function(e){for(var n=Math.random(),a='<div><input type=\"radio\" name=\"l\" id=\"l'+n+'\" onclick=\"zl(this)\"><label for=\"l'+n+'\">',l=0;l<4;l++)a+='<span style=\"background-color:#'+e[l]+'\"></span>';a+=e[4]+\"</label>\",_f.l&&(a+='<a href=\"#\" onclick=\"zd(this)\">X</a>'),a+=\"</div>\",t(s(\"#l\"),a)},window.p=function(){alert(\"Failure\")},window.q=function(e,n,a){var l=e.files[0],o=new FileReader;o.onerror=p,o.onload=function(o){a&&zz(l.name,\"a\"),e.value=\"\",zz(o.target.result,n)},o.readAsText(l)},window.rl=function(e){confirm(e.value+\". Are you sure?\")&&(zz(\"0\",\"f\"),location.reload())},window.u=function(e){var n=e.value;return\"\"===n||e.min&&1*n<1*e.min||e.max&&1*n>1*e.max},window.v=function(e){return w(e,2)},window.w=function(e,n){for(e+=\"\";e.length<n;)e=\"0\"+e;return e},window.x=function(e){var n=new Date,a=v(n.getHours()),l=v(n.getMinutes()),o=v(n.getSeconds()),t=n.getFullYear(),u=n.getMonth(),i=v(n.getDate());e.value=t+\"-\"+v(u+1)+\"-\"+i+\"T\"+a+\":\"+l+\":\"+o,zz([\"Jan\",\"Feb\",\"Mar\",\"Apr\",\"May\",\"Jun\",\"Jul\",\"Aug\",\"Sep\",\"Oct\",\"Nov\",\"Dec\"][u]+\" \"+i+\" \"+t+a+\":\"+l+\":\"+o,\"t\")},window.y=function(e,n){u(e)||zz(w(e.value,4),n)},window.z=function(e,n){if(!u(e)){var a=e.value;_w.send(n+a)}},window.zz=function(e,n){z({value:e},n)},window.zl=function(e){zz(n(e),\"l\")},window.zd=function(e){zz(n(e),\"d\")},window.zm=function(e){z(e,\"m\")};"
#define UI_HTML_BODY              "<header><h1>WiFibonacci Clock Settings</h1></header><section><form><fieldset><legend>General</legend><input type=radio name=m id=m0 onclick=\"zm(this)\" value=0><label for=m0>Clock</label><br><input type=radio name=m id=m1 onclick=\"zm(this)\" value=1><label for=m1>Rainbow Cycle</label><br><input type=radio name=m id=m2 onclick=\"zm(this)\" value=2><label for=m2>Rainbow</label><br><input type=radio name=m id=m3 onclick=\"zm(this)\" value=3><label for=m3>Random</label><br><input type=radio name=m id=m4 onclick=\"zm(this)\" value=4><label for=m4>Pulse</label><br><input type=radio name=m id=m5 onclick=\"zm(this)\" value=5><label for=m5>Constant Light</label><br> Brightness: <input name=b type=range min=1 max=255 oninput=\"y(this,'b')\"><input type=number name=c min=1 max=255 oninput=\"y(this,'b')\"></fieldset><fieldset><legend>Clock</legend> Date and time:<br><input type=datetime-local name=t oninput=\"z(this,'t')\"><input type=button onclick=\"x(t)\" value=now><br> Palettes:<br><div class=h><span>off</span><span>hours</span><span>minutes</span><span>both</span></div><div id=l></div> Upload palette:<br><input type=file name=i><input type=button onclick=\"q(i,'c',1)\" value=upload></fieldset><fieldset><legend>Rainbow</legend> Delay (ms): <input name=r type=range min=1 max=100 oninput=\"y(this,'r')\"><input type=number name=s min=1 max=100 oninput=\"y(this,'r')\"></fieldset><fieldset><legend>Random</legend> Delay (ms): <input name=j type=range min=1 max=9999 oninput=\"y(this,'j')\"><input type=number name=k min=1 max=9999 oninput=\"y(this,'j')\"><br> Ease (ms): <input name=g type=range min=1 max=100 oninput=\"y(this,'g')\"><input type=number name=h min=1 max=100 oninput=\"y(this,'g')\"></fieldset><fieldset><legend>Pulse</legend> Color: <input name=p type=color oninput=\"z(this,'p')\"><br> Delay (ms): <input name=d type=range min=1 max=100 oninput=\"y(this,'p')\"><input type=number name=e min=1 max=100 oninput=\"y(this,'p')\"></fieldset><fieldset><legend>Constant light</legend> Color: <input name=f type=color oninput=\"z(this,'f')\"></fieldset><fieldset><legend>Settings</legend><input type=button onclick=\"dl()\" value=\"Download settings\"><input type=button onclick=\"rl(this)\" value=\"Restore factory settings\"><br> Load file:<br><input type=file name=n><input type=button onclick=\"q(n,'')\" value=load></fieldset></form></section><section class=help> If your WiFibonacci Clock briks, do a hard reset by maintaining the mode button pushed and then push the reset button.<br> Warning: this will delete all settings and palettes. </section><footer> &copy; 2017 Clément Ronzon </footer>"
#define UI_HTML_STYLE             "<style>html,body{font-family:Arial;font-size:14px;background:#fff;padding:3px;color:#000;margin:0;width:100%;line-height:2em;box-sizing:border-box}body.disabled{background-color:#ccc}section{width:350px;margin:0 auto}fieldset>legend{font-weight:bolder}label>span,div>span{display:inline-block;margin-right:2px}div.h{margin-left:22px}div>span{width:52px;text-align:center}label>span{width:50px;height:25px;border:1px solid black;vertical-align:bottom}header,footer{text-align:center}footer{color:#888;font-size:.75rem}#l a{margin-left:5px}.help{font-size:.8rem;font-style:italic;line-height:1em;margin:10px auto}</style>"

// Global variables
Settings _settings;
std::vector<Palette> _palettesV;
RtcDS3231<TwoWire> _clock(Wire);
Adafruit_NeoPixel _ledStrip = Adafruit_NeoPixel(LEDS_SIZE, LED_DATA, NEO_RGB + NEO_KHZ800);
ESP8266WebServer _server = ESP8266WebServer(80);
WebSocketsServer _webSocket = WebSocketsServer(81);
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

void setup(void) {
#if DEBUG
  initSerial();
#endif
  initFS();
  initButtons();
  initDS3231();
  initLedStrip();
  initAP();
  initWebSocket();
  initRandom();
  initServer();
}

void loop(void) {
  handleButtons();
  if (shouldRefreshLedStrip()) handleModes();
  _webSocket.loop();
  _server.handleClient();
}

