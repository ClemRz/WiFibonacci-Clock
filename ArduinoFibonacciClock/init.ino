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
 
void initSerial(void) {
  Serial.begin(9600);
  Serial.println();
  //Serial.setDebugOutput(true);
}

void initDS3231(void) {
  _clock.begin();
  RTCDateTime dt = _clock.getDateTime();
  if (dt.year == 2000) {
    _clock.setDateTime(__DATE__, __TIME__);
  }
#if DEBUG
  Serial.println(F("DS3231 init"));
  Serial.println(_clock.dateFormat("Y-m-d H:i:s", _clock.getDateTime()));
#endif
}

void initLedStrip(void) {
  _ledStrip.begin();
  _ledStrip.show();
}

void initRandom(void) {
  RTCDateTime dt = _clock.getDateTime();
  randomSeed(dt.unixtime);
#if DEBUG
  Serial.print(F("Random init. Number (0 to 100): ")); Serial.println(random(101));
#endif
}

void initButtons(void) {
  pinMode(BRIGHTNESS_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(PALETTE_BUTTON, INPUT_PULLUP);
}

