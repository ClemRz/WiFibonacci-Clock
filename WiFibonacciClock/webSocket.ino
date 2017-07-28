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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
      // How to:
      // send message to client:
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients:
      // webSocket.broadcastTXT("message here");
      // send message to client:
      // webSocket.sendBIN(num, payload, lenght);
  switch(type) {
    case WStype_DISCONNECTED:
#if DEBUG
      Serial.printf("[%u] Disconnected!\n", num);
#endif
      break;
    case WStype_CONNECTED:
      {
#if DEBUG
        IPAddress ip = _webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif
        sendContent(num);
        sendPalettes(num);
        sendSettings(num);
      }
      break;
    case WStype_TEXT:
#if DEBUG
      Serial.printf("[%u] get Text: %s\n", num, payload);
#endif
      char c1[8], c2[42];
      if (payload[1] == '#') {
        std::copy(payload + 2, payload + 8, c1);
        switch (payload[0]) {
          case 'f': //f#123abc
            loadFlashLightColor(c1);
            break;
          case 'p': //p#123abc
            loadPulseColor(c1);
            break;
        }
      } else {
        switch (payload[0]) {
          case 'a': //amondrian.json
            std::copy(payload + 1, payload + 9, c2); //TODO check what is the maximum length for the file name
            setNextFileName(c2);
          case 'b': //b001
            std::copy(payload + 1, payload + 4, c1);
            loadBrightness(atoi(c1));
            break;
          case 'c': //c["#ffffff","#ff0a0a","#f8de00","#0a0aff"]
            std::copy(payload + 1, payload + 42, c2);
            processPalette(num, c2);
            break;
          case 'l': //l01
            std::copy(payload + 1, payload + 3, c1);
            loadPalette(atoi(c1));
            break;
          case 'm': //m2
            std::copy(payload + 1, payload + 2, c1);
            loadMode(atoi(c1));
            break;
          case 'p': //p008
            std::copy(payload + 1, payload + 4, c1);
            loadPulseDelay(atoi(c1));
            break;
          case 'r': //r008
            std::copy(payload + 1, payload + 4, c1);
            loadRainbowDelay(atoi(c1));
            break;
          case 't': //tFeb 06 200901:04:05
            std::copy(payload + 1, payload + 12, c2); //date: Feb 06 2009
            std::copy(payload + 12, payload + 20, c1); //time: 01:04:05
            loadDateTime(c2, c1);
            break;
        }
      }
      break;
    case WStype_BIN:
#if DEBUG
      hexdump(payload, length);
#endif
      writeBinFile(_receivedFileName, payload, length);
      break;
#if DEBUG
    case WStype_ERROR:
    default:
      Serial.printf("[%u] WS error.", num);
#endif
  }
}

void setNextFileName(const char* name) {
  _receivedFileName = name;
}

void sendContent(uint8_t num) {
  _webSocket.sendTXT(num, UI_HTML_BODY);
  _webSocket.sendTXT(num, UI_HTML_STYLE);
}

void sendPalettes(uint8_t num) {
  char buffer[82]; // 41 + filename length //TODO check what is the maximum length for the file name
  for(std::vector<Palette>::iterator it = _palettesV.begin(); it != _palettesV.end(); ++it) {
    printPaletteJsonTo(*it, buffer, sizeof(buffer));
    _webSocket.sendTXT(num, buffer);
  }
}

void sendSettings(uint8_t num) {
  char buffer[159];
  printSettingsJsonTo(buffer, sizeof(buffer));
  _webSocket.sendTXT(num, buffer);
}

void processPalette(uint8_t num, char* palette) {
  if (writePalette(_receivedFileName, palette)) {
    if (loadPaletteJson(palette, getBaseName(_receivedFileName))) { //TODO duplicated names will appear several times, should we do a unicity check?
      _webSocket.sendTXT(num, "o");
      sendPalettes(num);
    }
  }
}

