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

void readPalettes(void) {
  uint8_t n = 0;
  while(readPalette(n)) yield();
}

bool readPalette(uint8_t n) {
  bool success = false;
  String file_path = (String)PALETTES_PATH + F("p") + n + F(".json");
#if DEBUG
  Serial.print(F("Reading ")); Serial.println(file_path);
#endif
  File file = SPIFFS.open(file_path, "r");
  if (file) {
    size_t size = file.size();
    if (size <= 1024) {
      std::unique_ptr<char[]> buf(new char[size]);
      file.readBytes(buf.get(), size);
      file.close();
      Parser::JsonParser<17> parser; // one palette is 1+4*(3+1)=17 tokens
      Parser::JsonArray p = parser.parse(buf.get());
      if (p.success()) {
        Palette palette;
        for (uint8_t i = 0; i < 4; i++) palette.colorAt[i] = _ledStrip.Color((int)p[i][0], (int)p[i][1], (int)p[i][2]);
        _palettesV.push_back(palette);
        success = true;
#if DEBUG
      } else {
        Serial.println(F("JSON parsing failed"));
#endif
      }
#if DEBUG
    } else {
      Serial.println(F("file size is too large"));
#endif
    }
#if DEBUG
  } else {
    Serial.println(F("file open failed"));
#endif
  }
  if (!success) {
    //_cfg.wakeUpRate = DEFAULT_WAKE_UP_RATE;
    //_cfg.doorStatus = VACANT;
    //strcpy(_cfg.url, DEFAULT_URL);
  }
  return success;
}

void readSettings(void) {
  
}

void writePalettes(void) {
  
}

void writeSettings(void) {
  
}

