#ifndef MOTOR_H_
#define MOTOR_H_

#include "pin.h"

typedef struct {
	Pin pinDirection;
	uint16_t timerMax;
	uint8_t isInverted;
	volatile uint16_t* timerCounterRegister16;
	volatile uint8_t* timerCounterRegister8;
} Motor;

void Motor_init(
	Motor* motor,
	Pin pinDirection,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8,
	uint8_t isInverted);

void Motor_setDirection(Motor* motor, uint8_t direction);

void Motor_setSpeedPercent(Motor* motor, float percent);

#endif