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
  reading = !reading;
  if (reading != _lastButtonState[pin]) _lastDebounceTime = millis();

  if ((millis() - _lastDebounceTime) > DEBOUNCE_DELAY_MS) {
    if (reading != _currentButtonState[pin]) {
      _currentButtonState[pin] = reading;
      ret = reading;
      if (canChangeBrightness(pin) && !reading) switchLedStripStatus();
#if DEBUG
      Serial.print(F("Button ")); Serial.print(pin); Serial.print(F(" changed to ")); Serial.println(reading); 
#endif
    } else if (canChangeBrightness(pin) && reading && _timer1 < millis()) {
      fadeLedStrip();
      _timer1 = FADING_DELAY_MS + millis();
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
    loadMode(_settings.mode + 1);
  }
}

bool canChangeBrightness(int pin) {
  return !noBrightnessMode(_settings.mode) && pin == BRIGHTNESS_BUTTON;
}

