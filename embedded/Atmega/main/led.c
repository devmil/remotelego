#include "led.h"

void RgbLed_init(RgbLed* led, Pin pinRed, Pin pinGreen, Pin pinBlue) {
	SoftPwmPin ppRed;
	SoftPwmPin_init(&ppRed, pinRed);
	SoftPwmPin ppGreen;
	SoftPwmPin_init(&ppGreen, pinGreen);
	SoftPwmPin ppBlue;
	SoftPwmPin_init(&ppBlue, pinBlue);
	led->pinRed = ppRed;
	led->pinGreen = ppGreen;
	led->pinBlue = ppBlue;
}

void RgbLed_tick(RgbLed* led) {
	SoftPwmPin_tick(&led->pinRed);
	SoftPwmPin_tick(&led->pinGreen);
	SoftPwmPin_tick(&led->pinBlue);
}

void RgbLed_setColor(RgbLed* rgbLed, uint32_t color) {
	uint8_t b = (uint8_t)(color >> 0);
	uint8_t g = (uint8_t)(color >> 8);
	uint8_t r = (uint8_t)(color >> 16);
	
	SoftPwmPin_setValue(&rgbLed->pinRed, r);
	SoftPwmPin_setValue(&rgbLed->pinGreen, g);
	SoftPwmPin_setValue(&rgbLed->pinBlue, b);
}