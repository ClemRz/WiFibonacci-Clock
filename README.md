# WiFibonacci Clock

The WiFibonacci Clock is a modified version of the Fibonacci Clock from [Philippe Chrétien](http://basbrun.com/).

## Enhancements

The main enhancement is the replacement of the Atmega328 with an [ESP8266](http://www.esp8266.com) providing WiFi capability. This is game-changing as we can now virtualize part of the physical UI having the ESP acting as a [Websocket](https://en.wikipedia.org/wiki/WebSocket) server. Furthermore, the dematerialization of the UI allows more tuning options.

The list of existing modes that Philippe included in its design are:

 - Current Time
 - Rainbow Cycle
 - Rainbow
 - Error Code Display

In the current design I chose to remove the Error Code Display mode and add the following list of new modes:

 - Random
 - Pulse
 - Constant Light
 
 For each modes several settings can be tweaked, here is a screenshot of the virtual UI:
 
 ![Screenshot](res/client.png)
