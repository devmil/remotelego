#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>

typedef enum {
	PinMode_Output = 1,
	PinMode_Input = 0
} PinMode;

typedef enum {
	PinValue_High = 1,
	PinValue_Low = 0
} PinValue;

typedef struct {
	volatile uint8_t* definitionRegister;
	volatile uint8_t* portRegister;
	uint8_t offset;
} Pin;

typedef struct {
	Pin pin;
	uint8_t counter;
	uint8_t value;
	PinValue lastPinValue;
} SoftPwmPin;

void Pin_init(Pin* pin, volatile uint8_t* definitionRegister, volatile uint8_t* portRegister, uint8_t offset);

void Pin_setMode(Pin* pin, PinMode mode);

void Pin_setValue(Pin* pin, PinValue value);

PinValue Pin_getValue(Pin* pin);

void SoftPwmPin_init(SoftPwmPin* softPwmPin, Pin pin);

void SoftPwmPin_setValue(SoftPwmPin* softPwmPin, uint8_t value);

void SoftPwmPin_tick(SoftPwmPin* softPwmPin);

#endif