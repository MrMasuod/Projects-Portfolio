#include <Arduino.h>
#include "ultrasonic.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

unsigned long pulseDuration;
int pulse;
int distance;
int echoPinStatus;
int i;

void ultrasonic_init(struct Ultrasonic* ultrasonic) {
  DDRD |= (1 << ultrasonic->trigPin); // Set TRIG_PIN as output
  DDRD &= ~(1 << ultrasonic->echoPin); // Set ECHO_PIN as input

  // External interrupt on any logical change for ECHO_PIN
  EICRA |= (0 << ISC01) | (1 << ISC00);
  EIMSK |= (1 << INT0);

  // Configure Timer1 for measuring pulse duration
  TCCR1A = 0;
  TCCR1B = (1 << CS10); // No prescaler
}

int ultrasonic_measure(struct Ultrasonic* ultrasonic) {
  PORTD |= (1 << ultrasonic->trigPin); // Trigger the ultrasonic sensor
  _delay_us(15);
  PORTD &= ~(1 << ultrasonic->trigPin);
  _delay_ms(50);
  return distance;
}

void ultrasonic_set_val(void) {
    if (!i) {
    // Echo pin is low, it's a rising edge
    TCCR1B |= (1 << CS10);
    pulse = 0;
    i = 1;
  } else {
    // Echo pin is high, it's a falling edge
    TCCR1B = 0;
    pulse = TCNT1;
    distance = pulse / 58; // Conversion factor for distance calculation
    TCNT1 = 0;
    i = 0;
  }
}