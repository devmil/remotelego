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

typedef struct {
    Motor motor;
    float tickDuration;
    float remaining;
    float speedPercent;
} TimeoutMotor;

void TimeoutMotor_init(
    TimeoutMotor* tMotor, 
    float tickDuration,
    Pin pinDirection,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8, 
    uint8_t isInverted);

void TimeoutMotor_tick(TimeoutMotor* tMotor);

void TimeoutMotor_setRemaining(TimeoutMotor* tMotor, float remaining);

void TimeoutMotor_setDirection(TimeoutMotor* tMotor, uint8_t direction);

void TimeoutMotor_setSpeedPercent(TimeoutMotor* tMotor, float percent);

#endif