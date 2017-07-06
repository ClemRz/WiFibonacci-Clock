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

/*
 * This code is inspired from jarzebski's DS3231 library: https://github.com/jarzebski/Arduino-DS3231
 */
const uint8_t DAYS_ARRAY [] PROGMEM = {31,28,31,30,31,30,31,31,30,31,30,31};

char* dateFormat(const char* dateFormat, RtcDateTime dt) {
  char buffer[255];
  buffer[0] = 0;
  char helper[11];
  while (*dateFormat != '\0') {
    switch (dateFormat[0]) {
      // Day decoder
      case 'd':
        sprintf(helper, "%02d", dt.Day()); 
        strcat(buffer, (const char *)helper); 
        break;
      case 'j':
        sprintf(helper, "%d", dt.Day());
        strcat(buffer, (const char *)helper);
        break;
      case 'l':
        strcat(buffer, (const char *)strDayOfWeek(dt.DayOfWeek()));
        break;
      case 'D':
        strncat(buffer, strDayOfWeek(dt.DayOfWeek()), 3);
        break;
      case 'N':
        sprintf(helper, "%d", dt.DayOfWeek());
        strcat(buffer, (const char *)helper);
        break;
      case 'w':
        sprintf(helper, "%d", (dt.DayOfWeek() + 7) % 7);
        strcat(buffer, (const char *)helper);
        break;
      case 'z':
        sprintf(helper, "%d", dayInYear(dt.Year(), dt.Month(), dt.Day()));
        strcat(buffer, (const char *)helper);
        break;
      case 'S':
        strcat(buffer, (const char *)strDaySufix(dt.Day()));
        break;
  
      // Month decoder
      case 'm':
        sprintf(helper, "%02d", dt.Month());
        strcat(buffer, (const char *)helper);
        break;
      case 'n':
        sprintf(helper, "%d", dt.Month());
        strcat(buffer, (const char *)helper);
        break;
      case 'F':
        strcat(buffer, (const char *)strMonth(dt.Month()));
        break;
      case 'M':
        strncat(buffer, (const char *)strMonth(dt.Month()), 3);
        break;
      case 't':
        sprintf(helper, "%d", daysInMonth(dt.Year(), dt.Month()));
        strcat(buffer, (const char *)helper);
        break;
  
      // Year decoder
      case 'Y':
        sprintf(helper, "%d", dt.Year()); 
        strcat(buffer, (const char *)helper); 
        break;
      case 'y': sprintf(helper, "%02d", dt.Year()-2000);
        strcat(buffer, (const char *)helper);
        break;
      case 'L':
        sprintf(helper, "%d", isLeapYear(dt.Year())); 
        strcat(buffer, (const char *)helper); 
        break;
  
      // Hour decoder
      case 'H':
        sprintf(helper, "%02d", dt.Hour());
        strcat(buffer, (const char *)helper);
        break;
      case 'G':
        sprintf(helper, "%d", dt.Hour());
        strcat(buffer, (const char *)helper);
        break;
      case 'h':
        sprintf(helper, "%02d", hour12(dt.Hour()));
        strcat(buffer, (const char *)helper);
        break;
      case 'g':
        sprintf(helper, "%d", hour12(dt.Hour()));
        strcat(buffer, (const char *)helper);
        break;
      case 'A':
        strcat(buffer, (const char *)strAmPm(dt.Hour(), true));
        break;
      case 'a':
        strcat(buffer, (const char *)strAmPm(dt.Hour(), false));
        break;
  
      // Minute decoder
      case 'i': 
        sprintf(helper, "%02d", dt.Minute());
        strcat(buffer, (const char *)helper);
        break;
  
      // Second decoder
      case 's':
        sprintf(helper, "%02d", dt.Second()); 
        strcat(buffer, (const char *)helper); 
        break;
  
      // Misc decoder
      case 'U': 
        sprintf(helper, "%lu", dt.Epoch32Time());
        strcat(buffer, (const char *)helper);
        break;
  
      default: 
        strncat(buffer, dateFormat, 1);
        break;
    }
    dateFormat++;
  }
  return buffer;
}

char *strDayOfWeek(uint8_t dayOfWeek) {
  switch (dayOfWeek) {
    case 1:
      return "Monday";
      break;
    case 2:
      return "Tuesday";
      break;
    case 3:
      return "Wednesday";
      break;
    case 4:
      return "Thursday";
      break;
    case 5:
      return "Friday";
      break;
    case 6:
      return "Saturday";
      break;
    case 7:
      return "Sunday";
      break;
    default:
      return "Unknown";
  }
}

uint16_t dayInYear(uint16_t year, uint8_t month, uint8_t day) {
  uint16_t fromDate;
  uint16_t toDate;

  fromDate = date2days(year, 1, 1);
  toDate = date2days(year, month, day);

  return (toDate - fromDate);
}

char *strDaySufix(uint8_t day) {
  if (day % 10 == 1) {
    return "st";
  } else if (day % 10 == 2) {
    return "nd";
  }
  if (day % 10 == 3) {
    return "rd";
  }

  return "th";
}

char *strMonth(uint8_t month) {
  switch (month) {
    case 1:
      return "January";
      break;
    case 2:
      return "February";
      break;
    case 3:
      return "March";
      break;
    case 4:
      return "April";
      break;
    case 5:
      return "May";
      break;
    case 6:
      return "June";
      break;
    case 7:
      return "July";
      break;
    case 8:
      return "August";
      break;
    case 9:
      return "September";
      break;
    case 10:
      return "October";
      break;
    case 11:
      return "November";
      break;
    case 12:
      return "December";
      break;
    default:
      return "Unknown";
  }
}

bool isLeapYear(uint16_t year) {
  return (year % 4 == 0);
}

uint8_t hour12(uint8_t hour24) {
  if (hour24 == 0) {
    return 12;
  }
  if (hour24 > 12) {
    return (hour24 - 12);
  }
  return hour24;
}

char *strAmPm(uint8_t hour, bool uppercase) {
  if (hour < 12) {
    if (uppercase) {
      return "AM";
    } else {
      return "am";
    }
  } else {
    if (uppercase) {
      return "PM";
    } else {
      return "pm";
    }
  }
}

uint16_t date2days(uint16_t year, uint8_t month, uint8_t day) {
  year = year - 2000;
  uint16_t days16 = day;

  for (uint8_t i = 1; i < month; ++i) {
    days16 += pgm_read_byte(DAYS_ARRAY + i - 1);
  }

  if ((month == 2) && isLeapYear(year)) {
    ++days16;
  }

  return days16 + 365 * year + (year + 3) / 4 - 1;
}

uint8_t daysInMonth(uint16_t year, uint8_t month) {
  uint8_t days;
  days = pgm_read_byte(DAYS_ARRAY + month - 1);

  if ((month == 2) && isLeapYear(year)) {
    ++days;
  }

  return days;
}
