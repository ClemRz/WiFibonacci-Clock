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

#ifndef _OVERRIDES_H
#define _OVERRIDES_H

class MyWebSocketsServer: public WebSocketsServer {
  public:
    MyWebSocketsServer(uint16_t port, const char* payload);
    virtual void handleNonWebsocketConnection(WSclient_t * client); 
  protected:
    const char* _payload;
};

void MyWebSocketsServer::handleNonWebsocketConnection(WSclient_t * client) {
  client->tcp->write(_payload);
}

MyWebSocketsServer::MyWebSocketsServer(uint16_t port, const char* payload) : WebSocketsServer(port) {
  _payload = payload;
}

#endif  //_OVERRIDES_H
