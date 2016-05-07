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

		Motor_setDirection(motor, 1);		
		Motor_setSpeedPercent(motor, 0);
} 

void Motor_setDirection(Motor* motor, uint8_t direction) {
	motor->direction = direction;
	Pin_setValue(&motor->pinDirection, direction);
	Motor_adaptSignal(motor);
}

void Motor_setSpeedPercent(Motor* motor, float percent) {
	if(percent > 100) {
		percent = 100;
	}
	if(percent < 0) {
		percent = 0;
	}
	motor->speedPercent = percent;
	Motor_adaptSignal(motor);
}

void Motor_adaptSignal(Motor* motor) {
	float percent = motor->speedPercent;

	if(motor->direction == 1) {
		//reverse signal as we are having a 1 as reference
		percent = 100 - percent;
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

void TimeoutMotor_init(
    TimeoutMotor* tMotor, 
    float tickDuration,
    Pin pinDirection,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8, 
    uint8_t isInverted) {
    
    Motor_init(&tMotor->motor, pinDirection, timerMax, timerCounterRegister16, timerCounterRegister8, isInverted);
    tMotor->tickDuration = tickDuration;
    tMotor->remaining = 0;
    tMotor->speedPercent = 0;
}

void TimeoutMotor_tick(TimeoutMotor* tMotor) {
    if(tMotor->remaining == 0) {
        return;
    }
    tMotor->remaining -= tMotor->tickDuration;
    if(tMotor->remaining <= 0) {
        Motor_setSpeedPercent(&tMotor->motor, 0);
        tMotor->remaining = 0;
    }
}

void TimeoutMotor_setRemaining(TimeoutMotor* tMotor, float remaining) {
    tMotor->remaining = remaining;
    if(tMotor->remaining > 0) {
        Motor_setSpeedPercent(&tMotor->motor, tMotor->speedPercent);
    }
}

void TimeoutMotor_setDirection(TimeoutMotor* tMotor, uint8_t direction) {
    Motor_setDirection(&tMotor->motor, direction);
}

void TimeoutMotor_setSpeedPercent(TimeoutMotor* tMotor, float percent) {
    tMotor->speedPercent = percent;
    if(tMotor->remaining > 0) {
        Motor_setSpeedPercent(&tMotor->motor, percent);
    }
}