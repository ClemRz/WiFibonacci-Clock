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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch(type) {
    case WStype_DISCONNECTED:
#if DEBUG
      Serial.printf("[%u] Disconnected!\n", num);
#endif
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = _webSocket.remoteIP(num);
#if DEBUG
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        //_webSocket.sendTXT(num, "Connected");
#endif
      }
      break;
    case WStype_TEXT:
#if DEBUG
      Serial.printf("[%u] get Text: %s\n", num, payload);
#endif
      if (payload[1] == '#') {
        char c[6];
        std::copy(payload + 2, payload + 8, c);
        switch (payload[0]) {
          case 'f':
            loadFlashLightColor(c);
            break;
          case 'p':
            loadPulseColor(c);
            break;
        }
      }
      // send message to client
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:
#if DEBUG
      Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
#endif
      hexdump(payload, lenght);
      // send message to client
      // webSocket.sendBIN(num, payload, lenght);
      break;
  }
}
