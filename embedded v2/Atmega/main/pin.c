#include "pin.h"

void Pin_init(Pin* pin, volatile uint8_t* definitionRegister, volatile uint8_t* portRegister, uint8_t offset) {
	pin->definitionRegister = definitionRegister;
	pin->portRegister = portRegister;
	pin->offset = offset;
}

void Pin_setMode(Pin* pin, uint8_t mode) {
	if(mode != 0) {
		*pin->definitionRegister |= (1 << pin->offset);
	} else {
		*pin->definitionRegister &= ~(1 << pin->offset);
	}
}

void Pin_setValue(Pin* pin, uint8_t value) {
	if(value != 0) {
		*pin->portRegister |= (1 << pin->offset);
	} else {
		*pin->portRegister &= ~(1 << pin->offset);
	}
}

void SoftPwmPin_init(SoftPwmPin* softPwmPin, Pin pin) {
	softPwmPin->counter = 0;
	softPwmPin->value = 0;
	softPwmPin->pin = pin;
	softPwmPin->lastPinValue = 0;
	Pin_setMode(&pin, 1);
	Pin_setValue(&pin, 0);
}

void SoftPwmPin_setValue(SoftPwmPin* softPwmPin, uint8_t value) {
	softPwmPin->value = value;
}

void SoftPwmPin_tick(SoftPwmPin* softPwmPin) {
	softPwmPin->counter++;
	uint8_t newValue = (softPwmPin->counter > softPwmPin->value) ? 0 : 1;
	if(newValue != softPwmPin->lastPinValue) {
		Pin_setValue(&softPwmPin->pin, newValue);
		softPwmPin->lastPinValue = newValue;
	}
}