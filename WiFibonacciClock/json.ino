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
  loadPaletteJson(DEFAULT_PALETTE);
}

void loadDefaultSettings(void) {
  loadSettingsJson(DEFAULT_SETTINGS);
}
 
bool loadPaletteJson(char* json) {
#if DEBUG
  Serial.println(json);
#endif
  Parser::JsonParser<5> parser; // one palette is 1+4*1=5 tokens
  Parser::JsonArray p = parser.parse(json);
  if (p.success()) {
    Palette palette;
    for (uint8_t i = 0; i < 4; i++) palette.at[i] = getColorFromHex(p[i]);
    _palettesV.push_back(palette);
    return true;
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
  return false;
}

bool loadSettingsJson(char* json) {
#if DEBUG
  Serial.println(json);
#endif
  Parser::JsonParser<11> parser;
  Parser::JsonObject p = parser.parse(json);
  if (p.success()) {
    loadFlashLightColor(p["flashLightColor"]);
    loadPulseColor(p["pulse"]["color"]);
    _settings.rainbowDelay = (long)p["rainbowDelay"];
    _settings.pulseDelay = (long)p["pulse"]["delay"];
    return true;
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
  return false;
}


