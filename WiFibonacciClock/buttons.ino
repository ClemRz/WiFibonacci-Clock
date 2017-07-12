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
  bool ret = false, reading = digitalRead(pin);
  reading = BRIGHTNESS_BUTTON ? reading : !reading;
  if (reading != _lastButtonState[pin]) _lastDebounceTime = millis();

  if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {
    if (reading != _currentButtonState[pin]) {
      _currentButtonState[pin] = reading;
      ret = reading;
      if (canChangeBrightness(pin) && !reading) switchLedStripStatus();
#if DEBUG
      Serial.print(F("Button ")); Serial.print(pin); Serial.print(F(" changed to ")); Serial.println(reading); 
#endif
    } else if (canChangeBrightness(pin) && reading) {
      fadeLedStrip(FADING_DELAY_MS);
    }
  }

  _lastButtonState[pin] = reading;
  return ret;
}

void handleButtons(void) {
  if(debounce(BRIGHTNESS_BUTTON)) {
#if DEBUG
    Serial.print(F("Brightness: ")); Serial.println(_ledStripOn);
#endif
  } else
  if (debounce(MODE_BUTTON)) {
    if (_modeIndex == PULSE_MODE) restoreBrightness();
    _modeIndex = (_modeIndex + 1) % MODES_SIZE;
    if (_modeIndex == PULSE_MODE) backupBrightness();
    _refreshLedStrip = true;
#if DEBUG
    Serial.print(F("Mode: ")); Serial.println(_modeIndex);
#endif
  } else if (debounce(PALETTE_BUTTON)) {
    _paletteIndex = (_paletteIndex + 1) % _palettesV.size();
    _refreshLedStrip = true;
#if DEBUG
    Serial.print(F("Palette: ")); Serial.println(_paletteIndex);
#endif
  }
}

bool canChangeBrightness(int pin) {
  return _modeIndex != PULSE_MODE && pin == BRIGHTNESS_BUTTON;
}

