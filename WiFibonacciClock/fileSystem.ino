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

bool readPalettes(void) {
  bool success = false;
  Dir dir = SPIFFS.openDir(PALETTES_PATH);
  while(dir.next()){
    File palette = dir.openFile("r");
    success = success || readFile(palette, PALETTE_TYPE);
    palette.close();
    yield();
  }
  return success;
}

void writePalettes(void) {
  
}

bool readSettings(void) {
  bool success = false;
  File settings = SPIFFS.open(SETTINGS_FILE_PATH, "r");
  success = success || readFile(settings, SETTINGS_TYPE);
  settings.close();
  return success;
}

void writeSettings(void) {
  
}

bool readFile(File file, uint8_t type) {
  bool success = false;
  if (file) {
    String file_path = (String)PALETTES_PATH + file.name();
#if DEBUG
    Serial.print(F("Reading ")); Serial.println(file_path);
#endif
    size_t size = file.size();
    if (size <= 1024) {
      std::unique_ptr<char[]> buf(new char[size]);
      file.readBytes(buf.get(), size);
      file.close();
      switch (type) {
        case PALETTE_TYPE:
          success = loadPaletteJson(buf.get());
          break;
        case SETTINGS_TYPE:
          success = loadSettingsJson(buf.get());
          break;
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
  return success;
}

