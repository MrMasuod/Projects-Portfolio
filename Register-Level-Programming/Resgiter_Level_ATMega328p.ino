#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#define F_CPU 1000000UL // Assuming a 1MHz clock

#define TRIG_PIN 3 // Trigger pin (D2)
#define ECHO_PIN 2 // Echo pin (D3)

volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile unsigned long pulseDuration = 0;
volatile int pulse = 0;
volatile int i = 0;
volatile int distance = 0;
volatile int echoPinStatus = 0; // Flag to track echo pin status
volatile int MODE = 0; // Flag to track echo pin status

void setup() {
  Serial.begin(9600);
  DDRD |= (1 << TRIG_PIN); // Set TRIG_PIN as output
  DDRD &= ~(1 << ECHO_PIN); // Set ECHO_PIN as input
  DDRD |= (1 << PD5);    // Set Pin 5 as Output
  DDRD |= (1 << PD4);    // Set Pin 4 as Output
  EICRA |= (0 << ISC01) | (1 << ISC00); // Set INT0 to trigger on any edge (rising or falling)
  EIMSK |= (1 << INT0); // Enable INT0 external interrupt
  sei(); // Enable global interrupts
  TCCR1A = 0; // Reset Timer1 Control Registers

}

void checkPin6Status() {
    // Check if Pin 6 (Digital Pin 6) is ON or OFF
    if (PIND & (1 << PD6)) {
        // Pin 6 is ON
        MODE = 1; // Set mode variable to 1
    } else {
        // Pin 6 is OFF
        MODE = 0; // Set mode variable to 0
    }
}
void loop() {
  PORTD |= (1 << TRIG_PIN); // Trigger the ultrasonic sensor
  _delay_us(15);
  PORTD &= ~(1 << TRIG_PIN);
  _delay_ms(500); // Adjust delay between measurements
}

ISR(INT0_vect) {
  if (!i) {
    // Echo pin is low, it's a rising edge
    Serial.println("Interrupt Rising");
    TCCR1B |= (1 << CS10);
    pulse = 0;
    i = 1;
    } 
    else {
    // Echo pin is high, it's a falling edge
    Serial.println("Interrupt Falling");
    TCCR1B = 0;
    pulse = TCNT1;
    distance = pulse / 58; // Conversion factor for distance calculation
    TCNT1 = 0;
    i = 0;
  }
}

int main(void) {
  setup();
  checkPin6Status();
  while (1) {
    loop();
    Serial.print("Distance is: ");
    Serial.print(abs(distance));
    Serial.println("cm");
    if((abs(distance) < 200) & ~(MODE)) {
      // Set Pin 4 (Digital Pin 4) HIGH
      PORTD |= (1 << PD4);   // Set Pin 4 HIGH
    } 
    else if ((abs(distance) < 200) & (MODE)) {
      // Set Pin 5 (Digital Pin 5) HIGH
      PORTD |= (1 << PD5);   // Set Pin 5 HIGH
    }
    else {
      // Turn off Pin 4 (Digital Pin 4)
      PORTD &= ~(1 << PD4);  // Set Pin 4 LOW
      // Turn off Pin 5 (Digital Pin 5)
      PORTD &= ~(1 << PD5);  // Set Pin 5 LOW
    }
  }
  return 0;
}