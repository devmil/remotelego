#include "pin.h"

void Pin_init(Pin* pin, volatile uint8_t* definitionRegister, volatile uint8_t* portRegister, uint8_t offset) {
	pin->definitionRegister = definitionRegister;
	pin->portRegister = portRegister;
	pin->offset = offset;
}

void Pin_setMode(Pin* pin, PinMode mode) {
	if(pin->definitionRegister == 0) {
		return;
	}
	if(mode == PinMode_Output) {
		*pin->definitionRegister |= (1 << pin->offset);
	} else {
		*pin->definitionRegister &= ~(1 << pin->offset);
	}
}

void Pin_setValue(Pin* pin, PinValue value) {
	if(pin->definitionRegister == 0) {
		return;
	}
	if(value == PinValue_High) {
		*pin->portRegister |= (1 << pin->offset);
	} else {
		*pin->portRegister &= ~(1 << pin->offset);
	}
}

PinValue Pin_getValue(Pin* pin) {
	if(pin->definitionRegister == 0) {
		return 0;
	}
	return (PinValue)(*pin->portRegister & (1 << pin->offset)) >> pin->offset;	
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
	softPwmPin->value = (uint8_t)value;
}

void SoftPwmPin_tick(SoftPwmPin* softPwmPin) {
	softPwmPin->counter++;
	PinValue newValue = (softPwmPin->counter > softPwmPin->value) ? PinValue_Low : PinValue_High;
	if(newValue != softPwmPin->lastPinValue) {
		Pin_setValue(&softPwmPin->pin, newValue);
		softPwmPin->lastPinValue = newValue;
	}
}