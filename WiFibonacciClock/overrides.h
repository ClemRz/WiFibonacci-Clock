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
          "Content-Length: 720\r\n"
          "Connection: close\r\n"
          "\r\n"
          "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"initial-scale=1,maximum-scale=1,user-scalable=no\"><style>html,body{font-family:Arial,sans-serif;font-size: 14px;background:#fff;padding:3px;color:#000;margin:0;width:100%;}</style><script>var c=new WebSocket('ws://192.168.4.1:81/',['arduino']);c.onopen=function(){};c.onerror=function(e){};c.onmessage=function(e){var j=JSON.parse(e.data);document.getElementById('f').value='#'+j.flashLightColor;document.getElementById('p').value='#'+j.pulse.color;};</script></head><body>FlashLight color: <input id=\"f\" type=\"color\" oninput=\"c.send('f'+this.value)\"><br><br>Pulse color: <input id=\"p\" type=\"color\" oninput=\"c.send('p'+this.value)\"></body></html>");
}

MyWebSocketsServer::MyWebSocketsServer(uint16_t port) : WebSocketsServer(port) {}

#endif  //_OVERRIDES_H
