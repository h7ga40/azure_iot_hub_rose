
#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define PIN_A5        (19)
#define LED_BUILTIN   (25)

void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
int digitalRead(int pin);

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);

unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);

#endif // ARDUINO_H
