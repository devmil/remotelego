#include "motor.h"

void Motor_init(
	Motor* motor,
	Pin pinDirection,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8,
	uint8_t isInverted) {
		motor->pinDirection = pinDirection;
		motor->timerMax = timerMax;
		motor->timerCounterRegister16 = timerCounterRegister16;
		motor->timerCounterRegister8 = timerCounterRegister8;
		motor->isInverted = isInverted;
		
		Pin_setMode(&pinDirection, 1); //output
		Pin_setValue(&pinDirection, 0);
		
		Motor_setSpeedPercent(motor, 0);
} 

void Motor_setDirection(Motor* motor, uint8_t direction) {
	Pin_setValue(&motor->pinDirection, direction);
}

void Motor_setSpeedPercent(Motor* motor, float percent) {
	if(percent > 100) {
		percent = 100;
	}
	if(percent < 0) {
		percent = 0;
	}
	if(motor->isInverted) {
		percent = 100 - percent;
	}
	float value = (motor->timerMax * percent) / 100;
	if(motor->timerCounterRegister16 != 0) {
		*motor->timerCounterRegister16 = (uint8_t)value;
	} else if(motor->timerCounterRegister8 != 0) {
		*motor->timerCounterRegister8 = (uint8_t)value;
	}
}