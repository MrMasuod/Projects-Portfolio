#include <Arduino.h>
#include "lcd.h"
#include <util/delay.h>

void lcdInit(struct LCD* lcd) {
  // Set pin modes
  pinMode(lcd->RS, OUTPUT);
  pinMode(lcd->RW, OUTPUT);
  pinMode(lcd->E, OUTPUT);
  pinMode(lcd->D4, OUTPUT);
  pinMode(lcd->D5, OUTPUT);
  pinMode(lcd->D6, OUTPUT);
  pinMode(lcd->D7, OUTPUT);

  // Initialization sequence
  lcdCommand(lcd, 0x33);
  lcdCommand(lcd, 0x32);
  lcdCommand(lcd, 0x28);
  lcdCommand(lcd, 0x0C);
  lcdCommand(lcd, 0x01);
  _delay_ms(2);
}

void lcdCommand(struct LCD* lcd, unsigned char command) {
  // Write port first 4 bits
  digitalWrite(lcd->D4, (command >> 4) & 0x01);
  digitalWrite(lcd->D5, (command >> 5) & 0x01);
  digitalWrite(lcd->D6, (command >> 6) & 0x01);
  digitalWrite(lcd->D7, (command >> 7) & 0x01);

  // Enable high
  digitalWrite(lcd->E, HIGH);
  // RW low
  digitalWrite(lcd->RW, LOW);
  // RS low
  digitalWrite(lcd->RS, LOW);
  // Delay 2ms
  _delay_ms(2);
  // Enable low
  digitalWrite(lcd->E, LOW);

  // Write port next 4 bits
  digitalWrite(lcd->D4, command & 0x01);
  digitalWrite(lcd->D5, (command >> 1) & 0x01);
  digitalWrite(lcd->D6, (command >> 2) & 0x01);
  digitalWrite(lcd->D7, (command >> 3) & 0x01);

  // Enable high
  digitalWrite(lcd->E, HIGH);
  // RW low
  digitalWrite(lcd->RW, LOW);
  // RS low
  digitalWrite(lcd->RS, LOW);
  // Delay 2ms
  _delay_ms(2);
  // Enable low
  digitalWrite(lcd->E, LOW);
}

void lcdData(struct LCD* lcd, unsigned char data) {
  // Write port first 4 bits
  digitalWrite(lcd->D4, (data >> 4) & 0x01);
  digitalWrite(lcd->D5, (data >> 5) & 0x01);
  digitalWrite(lcd->D6, (data >> 6) & 0x01);
  digitalWrite(lcd->D7, (data >> 7) & 0x01);

  // Enable high
  digitalWrite(lcd->E, HIGH);
  // RW low
  digitalWrite(lcd->RW, LOW);
  // RS high
  digitalWrite(lcd->RS, HIGH);
  // Delay 2ms
  _delay_ms(2);
  // Enable low
  digitalWrite(lcd->E, LOW);

  // Write port next 4 bits
  digitalWrite(lcd->D4, data & 0x01);
  digitalWrite(lcd->D5, (data >> 1) & 0x01);
  digitalWrite(lcd->D6, (data >> 2) & 0x01);
  digitalWrite(lcd->D7, (data >> 3) & 0x01);

  // Enable high
  digitalWrite(lcd->E, HIGH);
  // RW low
  digitalWrite(lcd->RW, LOW);
  // RS high
  digitalWrite(lcd->RS, HIGH);
  // Delay 2ms
  _delay_ms(2);
  // Enable low
  digitalWrite(lcd->E, LOW);
}

void lcdWriteString(struct LCD* lcd, const char* str) {
  while (*str != '\0') {
    lcdData(lcd, *str);
    str++;
  }
}

void lcdWriteNumber(struct LCD* lcd, int num) {
  char buffer[10]; // Buffer to store the string representation of the number
  itoa(num, buffer, 10); // Convert integer to string
  lcdWriteString(lcd, buffer);
}
