#ifndef MOTOR_H_
#define MOTOR_H_

#include "pin.h"

typedef enum {
	MotorMode_DirectPwm,
	MotorMode_LeftRightPwm
} MotorMode;

typedef struct {
	Pin pinDirection1;
	Pin pinDirection2;
	uint16_t timerMax;
	uint8_t isInverted;
	volatile uint16_t* timerCounterRegister16;
	volatile uint8_t* timerCounterRegister8;
	uint8_t direction;
	float speedPercent;
	MotorMode mode;
} Motor;

void Motor_init(
	Motor* motor,
	Pin pinDirection1,
	Pin pinDirection2,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8,
	uint8_t isInverted,
	MotorMode mode);

void Motor_setDirection(Motor* motor, uint8_t direction);

void Motor_setSpeedPercent(Motor* motor, float percent);

void Motor_adaptSignal(Motor* motor);

typedef struct {
    Motor motor;
    float tickDuration;
    float remaining;
    float speedPercent;
} TimeoutMotor;

void TimeoutMotor_init(
    TimeoutMotor* tMotor, 
    float tickDuration,
    Pin pinDirection1,
	Pin pinDirection2,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8, 
    uint8_t isInverted,
	MotorMode mode);

void TimeoutMotor_tick(TimeoutMotor* tMotor);

void TimeoutMotor_setRemaining(TimeoutMotor* tMotor, float remaining);

void TimeoutMotor_setDirection(TimeoutMotor* tMotor, uint8_t direction);

void TimeoutMotor_setSpeedPercent(TimeoutMotor* tMotor, float percent);

#endif