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

#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_NeoPixel.h>

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
// ======================================

// Pins
#define BRIGHTNESS_BUTTON     5
#define MODE_BUTTON           6
#define PALETTE_BUTTON        7
#define LED_DATA              8               // Pin connected to strip's data wire

// Serial
#define DS3231_I2C_ADDRESS    0x68            // RTC I2C address

// Buttons
#define BUTTONS_SIZE          9               // Max button pin + 1
#define DEBOUNCE_DELAY_MS     10L

// Clock
#define PALETTES_SIZE         10
#define MODES_SIZE            4
#define LEDS_SIZE             9               // Number of LEDs
#define CLOCK_PIXELS          5               // Number of fisical cells
#define RAINBOW_DELAY_MS      20

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
unsigned long _lastDebounceTime = 0;
uint8_t _brightness = 255;
int
  _palette = 0,
  _mode = 0,
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
DS3231 _clock;
Adafruit_NeoPixel _ledStrip = Adafruit_NeoPixel(LEDS_SIZE, LED_DATA, NEO_RGB + NEO_KHZ800);

// Global constants
const uint32_t
  _black = _ledStrip.Color(0, 0, 0),
  _white = _ledStrip.Color(255, 255, 255),
  _colors[PALETTES_SIZE][4] = {
{     // #1 RGB
  _white,                          // off
  _ledStrip.Color(255, 10, 10),    // hours
  _ledStrip.Color(10, 255, 10),    // minutes
  _ledStrip.Color(10, 10, 255)     // both;
}, {  // #2 Mondrian
  _white,                          // off
  _ledStrip.Color(255, 10, 10),    // hours
  _ledStrip.Color(248, 222, 0),    // minutes
  _ledStrip.Color(10, 10, 255)     // both;
}, {  // #3 Basbrun
  _white,                          // off
  _ledStrip.Color(80, 40, 0),      // hours
  _ledStrip.Color(20, 200, 20),    // minutes
  _ledStrip.Color(255, 100, 10)    // both;
}, {  // #4 80's
  _white,                          // off
  _ledStrip.Color(245, 100, 201),  // hours
  _ledStrip.Color(114, 247, 54),   // minutes
  _ledStrip.Color(113, 235, 219)   // both;
}, {  // #5 Pastel
  _white,                          // off
  _ledStrip.Color(255, 123, 123),  // hours
  _ledStrip.Color(143, 255, 112),  // minutes
  _ledStrip.Color(120, 120, 255)   // both;
}, {  // #6 Modern
  _white,                          // off
  _ledStrip.Color(212, 49, 45),    // hours
  _ledStrip.Color(145, 210, 49),   // minutes
  _ledStrip.Color(141, 95, 224)    // both;
}, {  // #7 Cold
  _white,                          // off
  _ledStrip.Color(209, 62, 200),   // hours
  _ledStrip.Color(69, 232, 224),   // minutes
  _ledStrip.Color(80, 70, 202)     // both;
}, {  // #8 Warm
  _white,                          // off
  _ledStrip.Color(237, 20, 20),    // hours
  _ledStrip.Color(246, 243, 54),   // minutes
  _ledStrip.Color(255, 126, 21)    // both;
}, {  // #9 Earth
  _white,                          // off
  _ledStrip.Color(70, 35, 0),      // hours
  _ledStrip.Color(70, 122, 10),    // minutes
  _ledStrip.Color(200, 182, 0)     // both;
}, {  // #10 Dark
  _white,                          // off
  _ledStrip.Color(211, 34, 34),    // hours
  _ledStrip.Color(80, 151, 78),    // minutes
  _ledStrip.Color(16, 24, 149)     // both;
}};

void setup(void) {
#if DEBUG
  initSerial();
#endif
  initButtons();
  initDS3231();
  initLedStrip();
  initRandom();
}

void loop(void) {
  handleButtons();
  handleModes();
}

