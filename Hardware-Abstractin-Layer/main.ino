#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "ultrasonic.h"
#include "lcd.h"
#include "ultrasonic.c"
#include "lcd.c"


// Define the Ultrasonic struct
struct Ultrasonic ultrasonicSensor;
int dist;
// Define the LCD struct
struct LCD lcd;

int main(void) {
  // Initialize Ultrasonic sensor
  ultrasonicSensor.trigPin = 3; // TRIG_PIN
  ultrasonicSensor.echoPin = 2; // ECHO_PIN
  ultrasonic_init(&ultrasonicSensor);

  // Initialize LCD
  lcd.RS = 8;
  lcd.RW = 9;
  lcd.E = 10;
  lcd.D4 = 11;
  lcd.D5 = 12;
  lcd.D6 = 13;
  lcd.D7 = A0;
  lcdInit(&lcd);

  sei(); // Enable global interrupts

  while (1) {
    // Measure distance with Ultrasonic sensor
    dist = ultrasonic_measure(&ultrasonicSensor);
    // Display distance on LCD
    lcdCommand(&lcd, 0x01); // Clear the display
    lcdWriteString(&lcd, "Distance: ");
    lcdWriteNumber(&lcd, abs(dist));
    lcdWriteString(&lcd, " cm");

    _delay_ms(1000);
  }

  return 0;
}


ISR(INT0_vect) {
  ultrasonic_set_val();
}