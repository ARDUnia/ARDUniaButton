#ifndef ARDUINO_H
#define ARDUINO_H
#include <stdint.h>
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
// Deliberately no INPUT_PULLDOWN: simulates Arduino UNO.
uint32_t millis();
void pinMode(uint8_t, uint8_t);
int digitalRead(uint8_t);
#endif
