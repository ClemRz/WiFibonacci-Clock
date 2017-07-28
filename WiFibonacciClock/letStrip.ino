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

void handleModes(void) {
  switch (_settings.mode) {
    case CLOCK_MODE:
      displayCurrentTime();
      break;
    case RAINBOW_CYCLE_MODE:
      rainbowCycle(_settings.rainbowDelay);
      break;
    case RAINBOW_MODE:
      rainbow(_settings.rainbowDelay);
      break;
    case PULSE_MODE:
      pulse(_settings.pulseColor, _settings.pulseDelay);
    case FLASH_LIGHT_MODE:
      flashLight(_settings.flashLightColor);
      break;
  }
}

void displayCurrentTime() {
  RtcDateTime dt = _clock.GetDateTime();
  setTime(dt.Hour() % 12, dt.Minute());
}

void rainbowCycle(uint8_t delayMs) {
  for (uint8_t i = 0; i < CLOCK_PIXELS; i++) setPixel(i, wheel(((i * 256 / CLOCK_PIXELS) + _j) & 255));
  _ledStrip.show();
  _j = (_j + 1) % (256 * 5);
  delay(delayMs);
}

void rainbow(uint8_t delayMs) {
  for(uint8_t i = 0; i < CLOCK_PIXELS; i++) setPixel(i, wheel((i + _j) & 255));
  _ledStrip.show();
  _j = (_j + 1) % 256;
  delay(delayMs);
}

void flashLight(uint32_t color) {
  if (_refreshLedStrip) {
    for (uint8_t i= 0; i < CLOCK_PIXELS; i++) setPixel(i, color);
    _ledStrip.show();
    _refreshLedStrip = false;
  }
}

void pulse(uint32_t color, unsigned long delayMs) {
  for (uint8_t i= 0; i < CLOCK_PIXELS; i++) setPixel(i, color);
  switchLedStripStatus();
  fadeLedStrip(delayMs);
}

void loadMode(uint8_t index) {
  index = index % MODES_SIZE;
  if (_settings.mode != index) {
    if (index == PULSE_MODE) backupBrightness();
    if (_settings.mode == PULSE_MODE) restoreBrightness();
  }
  _settings.mode = index;
  _refreshLedStrip = true;
#if DEBUG
  Serial.print(F("Mode: ")); Serial.println(_settings.mode);
#endif
}

void loadPalette(uint8_t index) {
    index = index % _palettesV.size();
    _settings.palette = index;
    _refreshLedStrip = true;
#if DEBUG
    Serial.print(F("Palette: ")); Serial.println(_settings.palette);
#endif
}

void loadPulseDelay(long delay) {
  _settings.pulseDelay = delay;
}

void loadRainbowDelay(long delay) {
  _settings.rainbowDelay = delay;
}

void loadFlashLightColor(char* hexColor) {
  _settings.flashLightColor = hexToDec(hexColor);
  _refreshLedStrip = true;
}

void loadPulseColor(char* hexColor) {
  _settings.pulseColor = hexToDec(hexColor);
  _refreshLedStrip = true;
}

void loadBrightness(uint8_t brightness) {
  _settings.brightness = brightness;
  if (_settings.mode == PULSE_MODE) return;
  applyCorrectedBrightness();
}

void backupBrightness(void) {
  _brightnessBackup = _ledStrip.getBrightness();
}

void restoreBrightness(void) {
  _ledStrip.setBrightness(_brightnessBackup);
}

void fadeLedStrip(unsigned long delayMs) {
  if (_ledStripOn) {
    fadeLedStripOff(delayMs);
  } else {
    fadeLedStripOn(delayMs);
  }
}

void fadeLedStripOff(uint8_t delayMs) {
  _settings.brightness = _settings.brightness > FADING_STEP ? _settings.brightness - FADING_STEP : 0;
  applyCorrectedBrightness();
  delay(delayMs);
}

void fadeLedStripOn(uint8_t delayMs) {
  _settings.brightness = _settings.brightness < 255 - FADING_STEP ? _settings.brightness + FADING_STEP : 255;
  applyCorrectedBrightness();
  delay(delayMs);
}

void applyCorrectedBrightness(void) {
  uint8_t b = gammaCorrect(_settings.brightness);
  _ledStrip.setBrightness(b == 0 ? 1 : b);
  _ledStrip.show();
}

void switchLedStripStatus(void) {
  switch (_ledStrip.getBrightness()) {
    case 1:
      _ledStripOn = false;
      break;
    case 255:
      _ledStripOn = true;
      break;
  }
}

uint8_t gammaCorrect(uint8_t p) {
  return pgm_read_byte(&GAMMA_8[p]);
}

uint32_t wheel(byte wheelPos) {
  if (wheelPos < 85) {
    return _ledStrip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if(wheelPos < 170) {
    wheelPos -= 85;
    return _ledStrip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
    wheelPos -= 170;
    return _ledStrip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

void setTime(byte hours, byte minutes) {
  if (_hours != hours || _minutes/5 != minutes/5 || _refreshLedStrip) {
#if DEBUG
    Serial.print(hours); Serial.print(F(":")); Serial.println(minutes);
#endif
    _refreshLedStrip = false;
    _hours = hours;
    _minutes = minutes;
    
    for (int i=0; i<CLOCK_PIXELS; i++) _bits[i] = 0;
    
    setBits(hours, 0x01);
    setBits(minutes/5, 0x02);
    
    for (int i=0; i<CLOCK_PIXELS; i++) {
      setPixel(i, _palettesV.at(_settings.palette).at[_bits[i]]);
    }
    _ledStrip.show();
  }
}

void setPixel(byte pixel, uint32_t color) {
  switch (pixel) {
    case 0:
      _ledStrip.setPixelColor(0, color);
      break;
    case 1:
      _ledStrip.setPixelColor(1, color);
      break;
    case 2:
      _ledStrip.setPixelColor(2, color);
      break;
    case 3:
      _ledStrip.setPixelColor(3, color);
      _ledStrip.setPixelColor(4, color);
      break;
    case 4:
      _ledStrip.setPixelColor(5, color);
      _ledStrip.setPixelColor(6, color);
      _ledStrip.setPixelColor(7, color);
      _ledStrip.setPixelColor(8, color);
      _ledStrip.setPixelColor(9, color);
      break;
  }
}

void setBits(byte value, byte offset) {
  switch (value) {
    case 1:
      switch (random(2)) {
        case 0:
          _bits[0] |= offset;
          break;
        case 1:
          _bits[1] |= offset;
          break;
      }
      break;
    case 2:
      switch (random(2)) {
        case 0:
          _bits[2] |= offset;
          break;
        case 1:
          _bits[0] |= offset;
          _bits[1] |= offset;
          break;
      }
      break;
    case 3:
      switch (random(3)) {
        case 0:
          _bits[3] |= offset;
          break;
        case 1:
          _bits[0] |= offset;
          _bits[2] |= offset;
          break;
        case 2:
          _bits[1] |= offset;
          _bits[2] |= offset;
          break;
      }
      break;
    case 4:
      switch (random(3)) {
        case 0:
          _bits[0] |= offset;
          _bits[3] |= offset;
          break;
        case 1:
          _bits[1] |= offset;
          _bits[3] |= offset;
          break;
        case 2:
          _bits[0] |= offset;
          _bits[1] |= offset;
          _bits[2] |= offset;
          break;
      }
      break;
    case 5:
      switch (random(3)) {
        case 0:
          _bits[4] |= offset;
          break;
        case 1:
          _bits[2] |= offset;
          _bits[3] |= offset;
          break;
        case 2:
          _bits[0] |= offset;
          _bits[1] |= offset;
          _bits[3] |= offset;
          break;
      }
      break;
    case 6:
      switch (random(4)) {
        case 0:
          _bits[0] |= offset;
          _bits[4] |= offset;
          break;
        case 1:
          _bits[1] |= offset;
          _bits[4] |= offset;
          break;
        case 2:
          _bits[0] |= offset;
          _bits[2] |= offset;
          _bits[3] |= offset;
          break;
        case 3:
          _bits[1] |= offset;
          _bits[2] |= offset;
          _bits[3] |= offset;
          break;
      }
      break;
    case 7:
      switch (random(3)) {
        case 0:
          _bits[2] |= offset;
          _bits[4] |= offset;
          break;
        case 1:
          _bits[0] |= offset;
          _bits[1] |= offset;
          _bits[4] |= offset;
          break;
        case 2:
          _bits[0] |= offset;
          _bits[1] |= offset;
          _bits[2] |= offset;
          _bits[3] |= offset;
          break;
      }
      break;
    case 8:
      switch (random(3)) {
        case 0:
          _bits[3] |= offset;
          _bits[4] |= offset;
          break;
        case 1:
          _bits[0] |= offset;
          _bits[2] |= offset;
          _bits[4] |= offset;
          break;
        case 2:
          _bits[1] |= offset;
          _bits[2] |= offset;
          _bits[4] |= offset;
          break;
      }      
      break;
    case 9:
      switch (random(2)) {
        case 0:
          _bits[0] |= offset;
          _bits[3] |= offset;
          _bits[4] |= offset;
          break;
        case 1:
          _bits[1] |= offset;
          _bits[3] |= offset;
          _bits[4] |= offset;
          break;
      }      
      break;
    case 10:
      switch (random(2)) {
        case 0:
          _bits[2] |= offset;
          _bits[3] |= offset;
          _bits[4] |= offset;
          break;
        case 1:
          _bits[0] |= offset;
          _bits[1] |= offset;
          _bits[3] |= offset;
          _bits[4] |= offset;
          break;
      }            
      break;
    case 11:
      switch (random(2)) {
        case 0:
          _bits[0] |= offset;
          _bits[2] |= offset;
          _bits[3] |= offset;
          _bits[4] |= offset;      
          break;
        case 1:
          _bits[1] |= offset;
          _bits[2] |= offset;
          _bits[3] |= offset;
          _bits[4] |= offset; 
          break;
      }          
      break;
    case 12:
      _bits[0] |= offset;
      _bits[1] |= offset;
      _bits[2] |= offset;
      _bits[3] |= offset;
      _bits[4] |= offset;        
      break;
  }
}

