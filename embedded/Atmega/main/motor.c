#include "motor.h"

void Motor_init(
	Motor* motor,
	Pin pinDirection1,
	Pin pinDirection2,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8,
	uint8_t isInverted,
	MotorMode mode) {
		motor->pinDirection1 = pinDirection1;
		motor->pinDirection2 = pinDirection2;
		motor->timerMax = timerMax;
		motor->timerCounterRegister16 = timerCounterRegister16;
		motor->timerCounterRegister8 = timerCounterRegister8;
		motor->isInverted = isInverted;
		motor->mode = mode;
		
		Pin_setMode(&pinDirection1, 1); //output
		Pin_setValue(&pinDirection1, PinValue_Low);

		Pin_setMode(&pinDirection2, 1); //output
		Pin_setValue(&pinDirection2, PinValue_Low);

		Motor_setDirection(motor, 1);		
		Motor_setSpeedPercent(motor, 0);
} 

void Motor_setDirection(Motor* motor, uint8_t direction) {
	motor->direction = direction;
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

	if(motor->mode == MotorMode_DirectPwm) {
		PinValue directionVal = PinValue_Low;
		directionVal = motor->direction == 0 ? PinValue_Low : PinValue_High;

		if(motor->direction == 1) {
			//reverse signal as we are having a 1 as reference
			percent = 100 - percent;
		}
		if(motor->speedPercent == 0) {
			//disable direction pin by setting it to the PWM value
			directionVal = motor->isInverted ? PinValue_High : PinValue_Low;
		}
		Pin_setValue(&motor->pinDirection1, directionVal);		
	} else if(motor->mode == MotorMode_SpeedDirection) {
		PinValue directionVal = PinValue_Low;
		directionVal = motor->direction == 0 ? PinValue_Low : PinValue_High;
		
		if(motor->speedPercent == 0) {
			//disable direction pin by setting it to the PWM value
			directionVal = motor->isInverted ? PinValue_High : PinValue_Low;
		}
		Pin_setValue(&motor->pinDirection1, directionVal);		
	} else if(motor->mode == MotorMode_LeftRightPwm) {
		PinValue directionLeft = PinValue_Low;
		PinValue directionRight = PinValue_Low;
		if(motor->direction == 0) {
			directionLeft = PinValue_High;
		} else {
			directionRight = PinValue_High;
		}
		if(motor->speedPercent == 0) {
			//disable direction pin by setting left and right to 0
			directionLeft = PinValue_Low;
			directionRight = PinValue_Low;
		}
		Pin_setValue(&motor->pinDirection1, directionLeft);
		Pin_setValue(&motor->pinDirection2, directionRight);
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
    Pin pinDirection1,
	Pin pinDirection2,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister16,
	volatile uint8_t* timerCounterRegister8, 
    uint8_t isInverted,
	MotorMode mode) {
    
    Motor_init(&tMotor->motor, pinDirection1, pinDirection2, timerMax, timerCounterRegister16, timerCounterRegister8, isInverted, mode);
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