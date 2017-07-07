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
  Parser::JsonParser<17> parser; // one palette is 1+4*(3+1)=17 tokens
  
  Parser::JsonArray p = parser.parse(DEFAULT_PALETTE);
  if (p.success()) {
    Palette palette;
    for (uint8_t i = 0; i < 4; i++) palette.at[i] = _ledStrip.Color((int)p[i][0], (int)p[i][1], (int)p[i][2]);
    _palettesV.push_back(palette);
#if DEBUG
  } else {
    Serial.println(F("JSON parsing failed"));
#endif
  }
}

bool loadSettingsJson(char* json) {
  _flashLightColor = _settings.flashLightColor;
}

void loadDefaultSettings(void) {
  _flashLightColor = _ledStrip.Color(255, 255, 255);
}

