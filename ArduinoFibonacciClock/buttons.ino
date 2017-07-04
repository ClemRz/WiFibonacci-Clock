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

bool debounce(int pin) {
  bool ret = false, reading = !digitalRead(pin);
  if (reading != _lastButtonState[pin]) _lastDebounceTime = millis();

  if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {
    if (reading != _currentButtonState[pin]) {
      _currentButtonState[pin] = reading;
      ret = reading;
      if (pin == BRIGHTNESS_BUTTON && !reading) {
        uint8_t b = _ledStrip.getBrightness();
        if (b == 1) _ledStripOn = false;
        if (b == 255) _ledStripOn = true;
      }
#if DEBUG
      Serial.print(F("Button ")); Serial.print(pin); Serial.print(F(" changed to ")); Serial.println(reading); 
#endif
    } else {
      if (pin == BRIGHTNESS_BUTTON && reading) {
        if (_ledStripOn) {
          fadeStripOff(RAINBOW_DELAY_MS);
        } else {
          fadeStripOn(RAINBOW_DELAY_MS);
        }
      }
    }
  }

  _lastButtonState[pin] = reading;
  return ret;
}

void handleButtons(void) {
#if DEBUG
  if(debounce(BRIGHTNESS_BUTTON)) {
    Serial.print(F("Toggle: ")); Serial.println(_ledStripOn);
  } else
#endif
  if (debounce(MODE_BUTTON)) {
    _mode = (_mode + 1) % MODES_SIZE;
    _refreshLedStrip = true;
#if DEBUG
    Serial.print(F("Mode: ")); Serial.println(_mode);
#endif
  } else if (debounce(PALETTE_BUTTON)) {
    _palette = (_palette + 1) % PALETTES_SIZE;
    _refreshLedStrip = true;
#if DEBUG
    Serial.print(F("Palette: ")); Serial.println(_palette);
#endif
  }
}

