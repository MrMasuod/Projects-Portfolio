#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

// Define the LCD struct
struct LCD {
  uint8_t RS;
  uint8_t RW;
  uint8_t E;
  uint8_t D4;
  uint8_t D5;
  uint8_t D6;
  uint8_t D7;
};

void lcdInit(struct LCD* lcd);
void lcdCommand(struct LCD* lcd, unsigned char command);
void lcdData(struct LCD* lcd, unsigned char data);
void lcdWriteString(struct LCD* lcd, const char* str);
void lcdWriteNumber(struct LCD* lcd, int num);

#endif
