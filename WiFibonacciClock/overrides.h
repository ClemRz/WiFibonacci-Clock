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
    MyWebSocketsServer(uint16_t port);
    virtual void handleNonWebsocketConnection(WSclient_t * client); 
};

void MyWebSocketsServer::handleNonWebsocketConnection(WSclient_t * client) {
  client->tcp->write("HTTP/1.1 200 OK\r\n"
          "Server: arduino-WebSocket-Server\r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: 384\r\n"
          "Connection: close\r\n"
          "\r\n"
          "<!DOCTYPE html><html><head><script>var c=new WebSocket('ws://192.168.4.1:81/',['arduino']);c.onopen=function(){},c.onerror=function(n){},c.onmessage=function(n){};</script></head><body>FlashLight color: <input type=\"color\" value=\"#ffffff\" oninput=\"c.send('f'+this.value)\"><br>Pulse color: <input type=\"color\" value=\"#ffb330\" oninput=\"c.send('p'+this.value)\"></body></html>");
}

MyWebSocketsServer::MyWebSocketsServer(uint16_t port) : WebSocketsServer(port) {}

#endif  //_OVERRIDES_H
