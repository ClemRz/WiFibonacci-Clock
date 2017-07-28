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
  Serial.setDebugOutput(true);
}

void initDS3231(void) {
#if DEBUG
  Serial.println(F("DS3231 init"));
#endif
  _clock.Begin();
  RtcDateTime compiledDateTime = RtcDateTime(__DATE__, __TIME__);
  if (!_clock.IsDateTimeValid()) {
#if DEBUG
    Serial.println(F("DS3231 lost confidence in the DateTime, assigning compilation date and time."));
    _clock.SetDateTime(compiledDateTime);
#endif
  }
  if (!_clock.GetIsRunning()) {
#if DEBUG
  Serial.println(F("DS3231 was not actively running, starting now"));
#endif
    _clock.SetIsRunning(true);
  }
  RtcDateTime now = _clock.GetDateTime();
  if (now < compiledDateTime) {
#if DEBUG
    Serial.println(F("DS3231 is older than compile time!  (Updating DateTime)"));
#endif
    _clock.SetDateTime(compiledDateTime);
#if DEBUG
  } else if (now > compiledDateTime) {
    Serial.println(F("DS3231 is newer than compile time. (this is expected)"));
  } else if (now == compiledDateTime) {
    Serial.println(F("DS3231 is the same as compile time! (not expected but all is fine)"));
#endif
  }
  _clock.Enable32kHzPin(false);
  _clock.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
#if DEBUG
  Serial.println(F("DS3231 init"));
  Serial.println(dateFormat("Y-m-d H:i:s", _clock.GetDateTime()));
#endif
}

void initLedStrip(void) {
  _ledStrip.begin();
  _ledStrip.show();
}

void initRandom(void) {
  RtcDateTime dt = _clock.GetDateTime();
  randomSeed(dt.Epoch32Time());
#if DEBUG
  Serial.print(F("Random init. Number (0 to 100): ")); Serial.println(random(101));
#endif
}

void initButtons(void) {
  pinMode(BRIGHTNESS_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
}

void initAP(void) {
  WiFi.disconnect();
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  IPAddress myIP = WiFi.softAPIP();
#if DEBUG
  Serial.print(F("AP IP address: ")); Serial.println(myIP);
#endif
}

void initWebSocket(void) {
  _webSocket.begin();
  _webSocket.onEvent(webSocketEvent);
}

void initFS(void) {
  SPIFFS.begin();
  if (!readPalettes()) {
#if DEBUG
    Serial.println(F("Loading default palette"));
#endif
    loadDefaultPalette();
  }
  if (!readSettings()) {
#if DEBUG
    Serial.println(F("Loading default settings"));
#endif
    loadDefaultSettings();
  }
}

