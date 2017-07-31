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

#ifndef _STRUCTURES_H
#define _STRUCTURES_H

typedef struct {
  struct {
    uint32_t flashLightColor;
    uint32_t rainbowDelay;
    uint32_t pulseColor;
    uint8_t pulseDelay;
    uint32_t randomDelay;
    uint8_t randomEase;
    uint8_t mode;
    uint8_t brightness;
    uint8_t palette;
  };
} Settings;

typedef struct {
  union {
    uint32_t at[4];
    struct {
      uint32_t off;
      uint32_t hours;
      uint32_t minutes;
      uint32_t both;
    };
  };
  String name;
} Palette;

#endif  //_STRUCTURES_H
