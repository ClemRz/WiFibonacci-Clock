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
          "Content-Length: 1099\r\n"
          "Connection: close\r\n"
          "\r\n"
          "<!DOCTYPE html><html><head><script>function sendRGB(){var n=parseInt(document.getElementById('r').value).toString(16),e=parseInt(document.getElementById('g').value).toString(16),o=parseInt(document.getElementById('b').value).toString(16);n.length<2&&(n='0'+n),e.length<2&&(e='0'+e),o.length<2&&(o='0'+o);var t='#'+n+e+o;console.log('RGB: '+t),connection.send(t)}var connection=new WebSocket('ws://192.168.4.1:81/',['arduino']);connection.onopen=function(){connection.send('Message from Browser to ESP8266 yay its Working!! '+new Date),connection.send('ping'),connection.send('Time: '+new Date)},connection.onerror=function(n){console.log('WebSocket Error ',n)},connection.onmessage=function(n){console.log('Server: ',n.data),connection.send('Time: '+new Date)};</script></head><body>LED Control:<br><br>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\"><br>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\"><br>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\"><br></body></html>");
}

MyWebSocketsServer::MyWebSocketsServer(uint16_t port) : WebSocketsServer(port) {}

#endif  //_OVERRIDES_H
