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
    success = success || readFile(palette, loadPaletteJson);
    palette.close();
    yield();
  }
  return success;
}

bool readSettings(void) {
  bool success = false;
#if DEBUG
    Serial.print(F("Opening: "));Serial.println(SETTINGS_FILE_PATH);
#endif
  File settings = SPIFFS.open(SETTINGS_FILE_PATH, "r");
  success = readFile(settings, loadSettingsJson);
  settings.close();
  return success;
}

void writeSettings(void) {
  
}

bool writePalette(String name, char* json) {
  return writeTxtFile((String)PALETTES_PATH + '/' + name, json);
}

void deletePalette(uint8_t index) {
  deleteFile((String)PALETTES_PATH + '/' + _palettesV.at(index).name + ".json");
}

void writeBinFile(String name, uint8_t * bin, size_t length) {
#if DEBUG
  Serial.println(F("Not implemented"));
#endif
}

bool writeTxtFile(String path, char* content) {
  File file = SPIFFS.open(path, "w");
  if (file) {
#if DEBUG
    Serial.print(F("Writing content: "));Serial.println(content);
#endif  //DEBUG
    file.print(content);
    file.close();
    return true;
#if DEBUG
  } else {
    Serial.print(F("file open failed: "));Serial.println(path);
#endif  //DEBUG
  }
  return false;
}

bool readFile(File file, std::function<bool (char* json, String name)> callback) {
  if (file) {
    String name = file.name();
#if DEBUG
    Serial.print(F("Reading ")); Serial.println(name);
#endif
    size_t size = file.size();
    if (size <= 1024) {
      std::unique_ptr<char[]> buf(new char[size]);
      file.readBytes(buf.get(), size);
      file.close();
      return callback(buf.get(), getBaseName(name));
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
  return false;
}

bool deleteFile(String path) {
  if (SPIFFS.remove(path)) {
    return true;
#if DEBUG
    Serial.print(path); Serial.println(F(" deleted"));
  } else {
    Serial.print(F("file delete failed: "));Serial.println(path);
#endif  //DEBUG
  }
  return false;
}

String getBaseName(String fileName) {
  return fileName.substring(fileName.lastIndexOf('/') + 1, fileName.lastIndexOf('.'));
}

