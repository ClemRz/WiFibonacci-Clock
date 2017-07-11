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
 
bool loadPaletteJson(char* json) {
  Parser::JsonParser<17> parser; // one palette is 1+4*(3+1)=17 tokens
  Parser::JsonArray p = parser.parse(json);
  if (p.success()) {
    Palette palette;
    for (uint8_t i = 0; i < 4; i++) palette.at[i] = _ledStrip.Color((int)p[i][0], (int)p[i][1], (int)p[i][2]);
    _palettesV.push_back(palette);
    return true;
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
  return false;
}

void loadDefaultPalette(void) {
  loadPaletteJson(DEFAULT_PALETTE);
}

bool loadSettingsJson(char* json) {
  Parser::JsonParser<17> parser;
  Parser::JsonObject p = parser.parse(json);
  if (p.success()) {
    _settings.flashLightColor = _ledStrip.Color((int)p["flashlightColor"][0], (int)p["flashlightColor"][1], (int)p["flashlightColor"][2]);
    _settings.rainbowDelay = (long)p["rainbowDelay"];
    _settings.pulseColor = _ledStrip.Color((int)p["pulse"]["color"][0], (int)p["pulse"]["color"][1], (int)p["pulse"]["color"][2]);
    _settings.pulseDelay = (long)p["pulse"]["delay"];
    return true;
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
  return false;
}

void loadDefaultSettings(void) {
  loadSettingsJson(DEFAULT_SETTINGS);
}

