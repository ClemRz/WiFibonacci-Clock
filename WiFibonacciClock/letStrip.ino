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

void handleModes(void) {
  //if (_ledStripOn) {
    //if (_ledStrip.getBrightness() < 255) {
    //  fadeStripOn(RAINBOW_DELAY_MS);
    //} else {
      switch (_mode) {
        case 0:
          displayCurrentTime();
          break;
        case 1:
          rainbowCycle(RAINBOW_DELAY_MS);
          break;
        case 2:
          rainbow(RAINBOW_DELAY_MS);
          break;
        case 3:
          flashlight();
          break;
      }
    //}
  //} else {
    //turnStripOff();
    //fadeStripOff(RAINBOW_DELAY_MS);
  //}
}

void displayCurrentTime() {
  RTCDateTime dt = _clock.getDateTime();
  setTime(dt.hour % 12, dt.minute);
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

void flashlight(void) {
  for (int i=0; i<CLOCK_PIXELS; i++) setPixel(i, _white);
  _ledStrip.show();
}

void fadeStripOff(uint8_t delayMs) {
  _brightness = _brightness > FADING_STEP ? _brightness - FADING_STEP : 0;
  uint8_t b = gammaCorrected(_brightness);
  _ledStrip.setBrightness(b == 0 ? 1 : b);
  _ledStrip.show();
  delay(delayMs);
}

void fadeStripOn(uint8_t delayMs) {
  _brightness = _brightness < 255 - FADING_STEP ? _brightness + FADING_STEP : 255;
  uint8_t b = gammaCorrected(_brightness);
  _ledStrip.setBrightness(b == 0 ? 1 : b);
  _ledStrip.show();
  delay(delayMs);
}

uint8_t gammaCorrected(uint8_t p) {
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
      setPixel(i, _colors[_palette][_bits[i]]);
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

