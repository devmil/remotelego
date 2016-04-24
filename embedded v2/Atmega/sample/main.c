#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <util/delay.h>
#include <stdlib.h>

#include "uart.h"

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

#define UART_BAUDRATE				19200

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
	Pin pinDirection;
	uint16_t timerMax;
	uint8_t isInverted;
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

void Motor_setSpeedPercent(Motor* motor, float percent);

void Motor_init(
	Motor* motor,
	Pin pinDirection,
	uint16_t timerMax,
	volatile uint16_t* timerCounterRegister,
	uint8_t isInverted) {
		motor->pinDirection = pinDirection;
		motor->timerMax = timerMax;
		motor->timerCounterRegister = timerCounterRegister;
		motor->isInverted = isInverted;
		
		Pin_setMode(&pinDirection, 1); //output
		Pin_setValue(&pinDirection, 0);
		
		Motor_setSpeedPercent(motor, 0);
} 

void Motor_setDirection(Motor* motor, uint8_t direction) {
	Pin_setValue(&motor->pinDirection, direction);
}

void Motor_setSpeedPercent(Motor* motor, float percent) {
	if(motor->isInverted) {
		percent = 100 - percent;
	}
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
	Pin pinDirection;
	pinDirection.definitionRegister = &DDRD;
	pinDirection.portRegister = &PORTD;
	pinDirection.offset = PD6;

	Motor_init(&s_mainMotor, pinDirection, MAIN_MOTOR_TIMER_TICKS, &OCR1A, 1);
}

int8_t s_test_direction = 1;
int8_t s_test_waitingCycles = 0;
int8_t s_test_isWaiting = 0;
float s_test_servo_percent = 50;

float s_test_motor_percent = 10;
float s_test_motor_percentageDirection = 1;
float s_test_motor_direction = 1;


void nextTestStep() {
	PORTD ^= (1<<PD7); 					// toggle PD7
		
	if(!s_test_isWaiting && s_test_servo_percent == 50) {
		if(s_test_waitingCycles == 0) {
			s_test_isWaiting = 1;
			s_test_waitingCycles = 6;
		}
	}
	if(s_test_waitingCycles == 0) {
		s_test_isWaiting = 0;
		s_test_servo_percent += s_test_direction * 10;
		if(s_test_servo_percent > 100) {
			s_test_direction *= -1;
			s_test_servo_percent = 100;
		} else if(s_test_servo_percent < 0) {
			s_test_direction *= -1;
			s_test_servo_percent = 0;
		}
		Servo_setPercent(s_test_servo_percent);
	} else {
		s_test_waitingCycles--;
	}

	s_test_motor_percent += s_test_motor_percentageDirection * 10;
	if(s_test_motor_percent > 100) {
		s_test_motor_percent = 100;
		s_test_motor_percentageDirection *= -1;
	} else if(s_test_motor_percent < -100) {
		s_test_motor_percent = -100;
		s_test_motor_percentageDirection *= -1;
	}

	Motor_setDirection(&s_mainMotor, s_test_motor_direction);
	if(s_test_motor_percent < 0) {
		Motor_setSpeedPercent(&s_mainMotor, -1 * s_test_motor_percent);			
	} else {
		Motor_setSpeedPercent(&s_mainMotor, s_test_motor_percent);			
	}
	if(s_test_motor_percent == 0) {
		if(s_test_motor_direction == 0) {
			s_test_motor_direction = 1;
		} else {
			s_test_motor_direction = 0;
		}
	}
		Duration duration = Clock_getDuration(&s_clock);
	
	char buffMinutes[16];
	itoa(duration.minutes, buffMinutes, 10);
	uart_puts(buffMinutes);
	uart_puts("m: ");
	char buffSeconds[16];
	itoa(duration.seconds, buffSeconds, 10);
	uart_puts(buffSeconds);
	uart_puts("s ");
	char buff[10];
	itoa((int)s_test_motor_percent, buff, 10);
	uart_puts("Motor: ");
	uart_puts(buff);
	uart_puts("%");
	uart_puts(", Servo: ");
	itoa((int)s_test_servo_percent, buff, 10);
	uart_puts(buff);
	uart_puts("%");
	uart_puts("\r\n");	
}

int main(void)
{	
	uart_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));
	DDRD |= (1<<PD7); 						// PD7 is output
	DDRC |= (1<<PC0); 						// PD6 is output
	
	PORTC |= (1 << PC0);
	
	Clock_init(&s_clock, SERVO_TIMER_PERIOD_MS);
	
	Servo_init();
	
	initMotorTimer();
	initMainMotor();
	
	TIMSK |= (1<<TOIE0);					// enable timer overflow interrupt 

	sei();
	
	uint16_t delay_ms = 500;
	uint16_t nextMilliseconds = 0;

	while (42)
	{
		Duration duration = Clock_getDuration(&s_clock);
		uint16_t totalMilliseconds = duration.seconds * 1000 + (uint16_t)duration.milliseconds;

		if(totalMilliseconds > nextMilliseconds) {
			nextTestStep();
			nextMilliseconds += delay_ms;
		} else {
			_delay_ms(10);
		}

	}
	return 0; 								// this line should never be reached
}