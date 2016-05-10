#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

typedef struct {
	volatile uint8_t* definitionRegister;
	volatile uint8_t* portRegister;
	uint8_t offset;
} Pin;

typedef struct {
	Pin pin;
	uint8_t counter;
	uint8_t value;
	uint8_t lastPinValue;
} SoftPwmPin;

void Pin_init(Pin* pin, volatile uint8_t* definitionRegister, volatile uint8_t* portRegister, uint8_t offset);

void Pin_setMode(Pin* pin, uint8_t mode);

void Pin_setValue(Pin* pin, uint8_t value);

uint8_t Pin_getValue(Pin* pin);

void SoftPwmPin_init(SoftPwmPin* softPwmPin, Pin pin);

void SoftPwmPin_setValue(SoftPwmPin* softPwmPin, uint8_t value);

void SoftPwmPin_tick(SoftPwmPin* softPwmPin);

#endif