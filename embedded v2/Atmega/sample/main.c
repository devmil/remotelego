#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

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

#define UART_BAUDRATE				38400

#define RECEIVER_BUFF_SIZE 128

typedef struct {
	double milliseconds;
	uint16_t seconds;
	uint16_t minutes;
	uint16_t hours;
	uint64_t days;
} Duration;

typedef struct {
	double tickDurationMs;
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

void uart_puti(uint32_t integer) {
	char buff[32];
	buff[31] = '\0';
	ltoa(integer, buff, 10);
	uart_puts(buff);
}

void Clock_init(Clock* clock, float tickDurationMs) {
	clock->tickDurationMs = tickDurationMs;
	Duration initDuration;
	initDuration.milliseconds = 0;
	initDuration.seconds = 0;
	initDuration.minutes = 0;
	initDuration.hours = 0;
	initDuration.days = 0;
	clock->duration = initDuration;
}

void Clock_tick(Clock* clock) {
	Duration duration = clock->duration;
	duration.milliseconds += clock->tickDurationMs;
	if(duration.milliseconds >= 1000) {
		duration.seconds++;
		duration.milliseconds -= 1000.0d;
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
	*motor->timerCounterRegister = (uint8_t)value;
}

void Servo_setPercent(float percent) {
	if(percent > 100) {
		percent = 100;
	}
	if(percent < 0) {
		percent = 0;
	}
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
}

char s_uartRcvBuffer[RECEIVER_BUFF_SIZE + 1];
uint8_t s_uartRcvBufferPos = 0;
uint8_t s_backslashReceived = 0;
uint8_t s_inCommand = 0;

uint8_t handleCommand(char* command, char* value) {
	uint8_t result = 0;
	if(strcmp(command, "ms") == 0) {
		double doubleVal = atof(value);
		Motor_setSpeedPercent(&s_mainMotor, (float)doubleVal);
		result = 1;
	} else if(strcmp(command, "md") == 0) {
		int intVal = atoi(value);
		Motor_setDirection(&s_mainMotor, intVal != 0 ? 1 : 0);
		result = 1;
	} else if(strcmp(command, "sp") == 0) {
		int intVal = atoi(value);
		Servo_setPercent(intVal);
		result = 1;
	}
	return result;
}

void handleCommands() {
	uint8_t inVal = 0;
	char commandBuff[20] = "";
	char valBuff[20] = "";
	uint8_t numOfCommands = 0;
	for(int i=0; i<s_uartRcvBufferPos; i++) {
		if(s_uartRcvBuffer[i] == '=') {
			inVal = 1;
			continue;			
		}
		else if(s_uartRcvBuffer[i] == '|') {
			if(handleCommand(commandBuff, valBuff)) {
				numOfCommands++;
			}
			commandBuff[0] = '\0';
			valBuff[0] = '\0';
			inVal = 0;
			continue;
		}
		else if(inVal) {
			uint16_t idx = strlen(valBuff);
			valBuff[idx] =  s_uartRcvBuffer[i];
			valBuff[idx + 1] = '\0';
		} else {
			uint16_t idx = strlen(commandBuff);
			commandBuff[idx] =  s_uartRcvBuffer[i];
			commandBuff[idx + 1] = '\0';
		}
	}
	if(handleCommand(commandBuff, valBuff)) {
		numOfCommands++;
	}
	uart_puts("\\r");
	uart_puts("nc=");
	char buff[12];
	buff[11] = '\0';
	itoa(numOfCommands, buff, 10);
	uart_puts(buff);
	uart_puts("/\r\n");
	
	s_inCommand = 0;
	s_uartRcvBufferPos = 0;
	
}

void cancelCommand() {
	s_inCommand = 0;
	s_uartRcvBufferPos = 0;
}

void handlePotentialCommand(char c) {
	if(!s_backslashReceived 
		&& !s_inCommand
		&& s_uartRcvBufferPos == 0) {
		//wait for the beginning
		if(c != '\\') {
			return;
		}
		s_backslashReceived = 1;
		return;
	}
	if(s_backslashReceived) {
		s_backslashReceived = 0;
		if(c != 'c' && c != 'C') {
			return;
		}
		s_inCommand = 1;
		return;
	}
	if(s_inCommand) {
		if(c == '/') {
			handleCommands();
			return;
		}
		s_uartRcvBuffer[s_uartRcvBufferPos] = c;
		s_uartRcvBufferPos++;
		if(s_uartRcvBufferPos >= RECEIVER_BUFF_SIZE) {
			cancelCommand();
		}
	}	
}

void handleUart() {
	uint8_t resultErrorCode = 0;
	do {
		cli();
		uint16_t getResult = uart_getc();
		sei();
		uint8_t c = (uint8_t)getResult;
		resultErrorCode = (uint8_t)(getResult >> 8);
		
		if (resultErrorCode == 0) {
			handlePotentialCommand(c);
		}
	} while(resultErrorCode == 0);
}

int main(void)
{
	char* reason="No known reason\r\n";	
	
	if(MCUCSR & (1<<PORF )) reason = "Power-on reset.\r\n";
	if(MCUCSR & (1<<EXTRF)) reason = "External reset!\r\n";
	if(MCUCSR & (1<<BORF )) reason = "Brownout reset!\r\n";
	if(MCUCSR & (1<<WDRF )) reason = "Watchdog reset!\r\n";
	if(MCUCSR & (1<<JTRF )) reason = "JTAG reset!\r\n";
	
	MCUCSR = 0;
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
	
	uart_puts(reason);
	uart_puts("Motor control ready\r\n");
	
	uint16_t delay_ms = 500;
	uint16_t nextMilliseconds = 0;
	uint8_t lastMinute = 0;

	wdt_enable(WDTO_120MS);

	while (42)
	{
		handleUart();
		Duration duration = Clock_getDuration(&s_clock);

		uint32_t totalMillisecondsThisMinute = (uint32_t)duration.milliseconds;
		totalMillisecondsThisMinute += (uint32_t)duration.seconds * (uint32_t)1000ul;
		
		if(lastMinute != duration.minutes) {
			lastMinute = duration.minutes;
			nextMilliseconds -= 60ul * 1000ul;
		}

		if(totalMillisecondsThisMinute > nextMilliseconds) {
			nextTestStep();
			nextMilliseconds += delay_ms;
		}
		_delay_ms(10);
		wdt_reset();
	}
	return 0; 								// this line should never be reached
}

ISR(BADISR_vect)
{

    for (;;) uart_puts("!");
}