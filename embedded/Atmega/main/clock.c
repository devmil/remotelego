#include "clock.h"

#include <avr/interrupt.h>

void Clock_init(Clock* clock, float tickDurationMs) {
	clock->tickDurationMs = tickDurationMs;
	Duration initDuration;
	initDuration.milliseconds = 0;
	initDuration.seconds = 0;
	initDuration.minutes = 0;
	initDuration.hours = 0;
	initDuration.days = 0;
	clock->duration = initDuration;
}

void Clock_tick(Clock* clock) {
	Duration duration = clock->duration;
	duration.milliseconds += clock->tickDurationMs;
	if(duration.milliseconds >= 1000) {
		duration.seconds++;
		duration.milliseconds -= 1000.0d;
		if(duration.seconds >= 60) {
			duration.minutes++;
			duration.seconds -= 60;
			if(duration.minutes >= 60) {
				duration.hours++;
				duration.minutes -= 60;
				if(duration.hours >= 24) {
					duration.days++;
					duration.hours -= 24;
				}
			}
		}
	}
	clock->duration = duration;
}

Duration Clock_getDuration(Clock* clock) {
	cli();
	Duration result = clock->duration;
	sei();
	return result;
}