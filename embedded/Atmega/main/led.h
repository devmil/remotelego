#ifndef LED_H_
#define LED_H_

#include "pin.h"

typedef struct {
	SoftPwmPin pinRed;
	SoftPwmPin pinGreen;
	SoftPwmPin pinBlue;
} RgbLed;

void RgbLed_init(RgbLed* led, Pin pinRed, Pin pinGreen, Pin pinBlue);

void RgbLed_tick(RgbLed* led);

void RgbLed_setColor(RgbLed* rgbLed, uint32_t color);

#endif