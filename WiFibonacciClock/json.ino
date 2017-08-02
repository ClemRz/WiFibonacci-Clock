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

void loadDefaultPalette(void) {
  loadPaletteJson(DEFAULT_PALETTE, "RGB");
}

void loadDefaultSettings(void) {
  loadSettingsJson(DEFAULT_SETTINGS, "");
}
 
bool loadPaletteJson(char* json, String fileName) {
#if DEBUG
  Serial.print(fileName);Serial.print(F(": "));Serial.println(json);
#endif
  Parser::JsonParser<5> parser; // one palette is 1+4*1=5 tokens
  Parser::JsonArray p = parser.parseArray(json);
  if (p.success()) {
    Palette palette;
    for (uint8_t i = 0; i < 4; i++) palette.at[i] = hexToDec((char*)p[i]);
    palette.name = fileName;
    _palettesV.push_back(palette);
    return true;
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
  return false;
}

bool loadSettingsJson(char* json, String unused) {
#if DEBUG
  Serial.println(json);
#endif
  Parser::JsonParser<27> parser;
  Parser::JsonObject p = parser.parse(json);
  if (p.success()) {
    loadFlashLightColor(p["flashLightColor"]);
    loadPulseColor(p["pulse"]["color"]);
    if (p.containsKey("date") && p.containsKey("time")) loadDateTime(p["date"], p["time"]);
    loadRainbowDelay(atoi(p["rainbowDelay"]));
    loadPulseDelay(atoi(p["pulse"]["delay"]));
    loadBrightness(atoi(p["brightness"]));
    loadMode(atoi(p["mode"]));
    loadPalette(atoi(p["palette"]));
    loadRandomDelay(atoi(p["random"]["delay"]));
    loadRandomEase(atoi(p["random"]["ease"]));
    return true;
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
  return false;
}

void printSettingsJsonTo(char* buffer, size_t bufferSize) {
  char buff1[7], buff2[7];
  Generator::JsonObject<2> pulse;
  decToHex(_settings.pulseColor, buff1, sizeof(buff1));
  pulse["color"] = buff1;
  pulse["delay"] = (long)_settings.pulseDelay;
  Generator::JsonObject<2> randm;
  randm["delay"] = (long)_settings.randomDelay;
  randm["ease"] = (long)_settings.randomEase;
  Generator::JsonObject<8> settings;
  decToHex(_settings.flashLightColor, buff2, sizeof(buff2));
  settings["dateTime"] = dateFormat("Y-m-dTH:i:s", _clock.GetDateTime());
  settings["flashLightColor"] = buff2;
  settings["rainbowDelay"] = (long)_settings.rainbowDelay;
  settings["mode"] = _settings.mode;
  settings["brightness"] = _settings.brightness;
  settings["palette"] = _settings.palette;
  settings["pulse"] = pulse;
  settings["random"] = randm;
  settings.printTo(buffer, bufferSize);
#if DEBUG
  settings.prettyPrintTo(Serial); Serial.println();
#endif
}

void printPaletteJsonTo(Palette palette, char* buffer, size_t bufferSize) {
  char buff[4][7];
  Generator::JsonArray<5> arr;
  for(uint8_t i = 0; i < 4; i++) {
    decToHex(palette.at[i], buff[i], sizeof(buff[i]));
    arr.add(buff[i]);
  }
  arr.add(palette.name.c_str());
  arr.printTo(buffer, bufferSize);
#if DEBUG
  arr.prettyPrintTo(Serial); Serial.println();
#endif
}

