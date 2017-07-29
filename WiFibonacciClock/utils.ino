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

uint32_t hexToDec(char* hex) {
  return strtol(hex, NULL, 16);
}

void decToHex(uint32_t dec, char* buffer, size_t bufferSize) {
  String hexString = String(dec, HEX);
  while (hexString.length() < bufferSize - 1) hexString = "0" + hexString;
  hexString.toCharArray(buffer, bufferSize);
}
