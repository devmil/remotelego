#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <util/delay.h>

#define SERVO_PRESCALER 			1024
#define SERVO_PRESCALER_BITS		(1<<CS00) | (1<<CS02)
#define SERVO_TIMER_TICKS			255
#define SERVO_EFFECTIVE_F			(F_CPU / SERVO_PRESCALER) //14745000 / 1024 = 14399
#define SERVO_TIMER_PERIOD_MS		(((float)1000 * SERVO_TIMER_TICKS) / SERVO_EFFECTIVE_F)	//(1000 * 255) / 14399 = 17,70956316410862
#define SERVO_TIMER_MS_PER_TICK		(SERVO_TIMER_PERIOD_MS / SERVO_TIMER_TICKS) //0,06944926731023
#define SERVO_MIN_MS				0.8f
#define SERVO_MAX_MS				2.3f

#define MAIN_MOTOR_PRESCALER		1
#define MAIN_MOTOR_PRESCALER_BITS	(1<<CS10)
#define MAIN_MOTOR_TIMER_TICKS		255

typedef struct {
	float milliseconds;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint64_t days;
} Duration;

typedef struct {
	float tickDurationMs;
	Duration duration;
} Clock;

typedef struct {
	volatile uint8_t* definitionRegister;
	volatile uint8_t* portRegister;
	uint8_t offset;
} Pin;

typedef struct {
	Pin pinEnable;
	Pin pinDirection;
	uint16_t timerMax;
	volatile uint16_t* timerCounterRegister;
} Motor;

void Clock_init(Clock* clock, float tickDurationMs) {
	clock->tickDurationMs = tickDurationMs;
}

void Clock_tick(Clock* clock) {
	Duration duration = clock->duration;
	duration.milliseconds += clock->tickDurationMs;
	if(duration.milliseconds >= 1000) {
		duration.seconds++;
		duration.milliseconds -= 1000;
		if(duration.seconds >= 60) {
			duration.minutes++;
			duration.seconds -= 60;
			if(duration.minutes >= 60) {
				duration.hours++;
				duration.minutes -= 60;
				if(duration.hours >= 24) {
					duration.days++;
					duration.hours -= 24;
				}
			}
		}
	}
	clock->duration = duration;
}

Duration Clock_getDuration(Clock* clock) {
	cli();
	Duration result = clock->duration;
	sei();
	return result;
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

void Motor_init(
	Motor* motor,
	Pin pinEnable,
	Pin pinDirection,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister) {
		motor->pinEnable = pinEnable;
		motor->pinDirection = pinDirection;
		motor->timerMax = timerMax;
		motor->timerCounterRegister = timerCounterRegister;
		
		Pin_setMode(&pinEnable, 1); //output
		Pin_setValue(&pinEnable, 0);
		Pin_setMode(&pinDirection, 1); //output
		Pin_setValue(&pinDirection, 0);
} 

void Motor_setDirection(Motor* motor, uint8_t direction) {
	Pin_setValue(&motor->pinDirection, direction);
}

void Motor_setEnable(Motor* motor, uint8_t enable) {
	Pin_setValue(&motor->pinEnable, enable);
}

void Motor_setSpeedPercent(Motor* motor, float percent) {
	float value = (motor->timerMax * percent) / 100;
	*motor->timerCounterRegister = (uint8_t)value;
}

void Servo_setPercent(float percent) {
	float servoMs = SERVO_MIN_MS + ((SERVO_MAX_MS - SERVO_MIN_MS) * percent) / 100;
	float servoTicks = servoMs / SERVO_TIMER_MS_PER_TICK;
	OCR0 = 255 - (int8_t)servoTicks;
}

void Servo_init() {
	DDRB |= (1<<PB3); 						// PB3 == OC0 is servo output
	PORTB |= (1<<PB3);						// pull down

	TCCR0 |= (1<<WGM00) | (1<<WGM01);		// fast PWM mode
	TCCR0 |= (1<<COM01);					// compare output mode
	TCCR0 |= (1<<COM00);					// non inverted PWM
	TCCR0 |= SERVO_PRESCALER_BITS;

	Servo_setPercent(50);					//center servo	
}

Clock s_clock;
Motor s_mainMotor;

ISR( TIMER0_OVF_vect )
{
	Clock_tick(&s_clock);
}

void initMotorTimer() {
	TCCR1A |= (1 << COM1A0) | (1 << COM1A1); //Timer1A: inverting PWM 
	TCCR1A |= (1 << WGM10); //Timer1A: Fast PWM mode bit 1
	TCCR1B |= (1 << WGM12); //Timer1A: Fast PWM mode bit 2
	TCCR1B |= MAIN_MOTOR_PRESCALER_BITS; //Prescaler = 1
	
	DDRD |= (1<<PD5); 						// PD5=OC1A is output
}

void initMainMotor() {
	Pin pinEnable;
	pinEnable.definitionRegister = &DDRA;
	pinEnable.portRegister = &PORTA;
	pinEnable.offset = PA0;

	Pin pinDirection;
	pinDirection.definitionRegister = &DDRA;
	pinDirection.portRegister = &PORTA;
	pinDirection.offset = PA1;

	Motor_init(&s_mainMotor, pinEnable, pinDirection, MAIN_MOTOR_TIMER_TICKS, &OCR1A);
}

int main(void)
{	
	DDRD |= (1<<PD7); 						// PD7 is output
	DDRD |= (1<<PD6); 						// PD6 is output
	
	PORTD |= (1 << PD6);
	
	Clock_init(&s_clock, SERVO_TIMER_MS_PER_TICK);
	
	Servo_init();
	
	initMotorTimer();
	initMainMotor();
	
	TIMSK |= (1<<TOIE0);					// enable timer overflow interrupt 

	int8_t direction = 1;
	int8_t waitingCycles = 0;
	int8_t isWaiting = 0;
	float servo_percent = 50;
	
	sei();
	
	Motor_setEnable(&s_mainMotor, 1);
	Motor_setDirection(&s_mainMotor, 0);
	
	while (42)
	{
		_delay_ms(500); 					// wait some time
		PORTD ^= (1<<PD7); 					// toggle PD7
		
		if(!isWaiting && servo_percent == 50) {
			if(waitingCycles == 0) {
				isWaiting = 1;
				waitingCycles = 6;
			}
		}
		if(waitingCycles == 0) {
			isWaiting = 0;
			servo_percent += direction * 10;
			if(servo_percent > 100) {
				direction *= -1;
				servo_percent = 100;
			} else if(servo_percent < 0) {
				direction *= -1;
				servo_percent = 0;
			}
			Servo_setPercent(servo_percent);
			Motor_setSpeedPercent(&s_mainMotor, servo_percent);
		} else {
			waitingCycles--;
		}
	}
	return 0; 								// this line should never be reached
}