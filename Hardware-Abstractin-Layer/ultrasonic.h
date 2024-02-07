#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

struct Ultrasonic {
  uint8_t trigPin;
  uint8_t echoPin;
};

extern unsigned long pulseDuration;
extern int pulse;
extern int distance;
extern int echoPinStatus;
extern int i;

void ultrasonic_init(struct Ultrasonic* ultrasonic);
int ultrasonic_measure(struct Ultrasonic* ultrasonic);
void ultrasonic_set_val(void);

#endif  // ULTRASONIC_H
