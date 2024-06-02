#include <Arduino.h>

// code is stolen from:
// https://www.e-tinkers.com/2020/01/do-you-know-arduino-sprintf-and-floating-point/
template <typename... T> void print(const char *str, T... args) {
  int len = snprintf(NULL, 0, str, args...);
  if (len) {
    char buff[len + 1];
    snprintf(buff, len + 1, str, args...);
    Serial.print(buff);
  }
}
