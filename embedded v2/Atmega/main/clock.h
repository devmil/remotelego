#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

typedef struct {
	double milliseconds;
	uint16_t seconds;
	uint16_t minutes;
	uint16_t hours;
	uint64_t days;
} Duration;

typedef struct {
	double tickDurationMs;
	Duration duration;
} Clock;


void Clock_init(Clock* clock, float tickDurationMs);

void Clock_tick(Clock* clock);

Duration Clock_getDuration(Clock* clock);

#endif