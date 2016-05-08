#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "clock.h"
#include "pin.h"
#include "led.h"
#include "motor.h"

#define SERVO_PRESCALER 			1024
#define SERVO_PRESCALER_BITS		(1<<CS00) | (1<<CS02)
#define SERVO_TIMER_TICKS			255
#define SERVO_EFFECTIVE_F			(F_CPU / SERVO_PRESCALER) //14745000 / 1024 = 14399
#define SERVO_TIMER_PERIOD_MS		(((float)1000 * SERVO_TIMER_TICKS) / SERVO_EFFECTIVE_F)	//(1000 * 255) / 14399 = 17,70956316410862
#define SERVO_TIMER_MS_PER_TICK		(SERVO_TIMER_PERIOD_MS / SERVO_TIMER_TICKS) //0,06944926731023
#define SERVO_MIN_MS				0.8f
#define SERVO_MAX_MS				2.3f

#define MAIN_MOTOR_PRESCALER		64
#define MAIN_MOTOR_PRESCALER_BITS	(1<<CS11) | (1<<CS10)
#define MAIN_MOTOR_TIMER_TICKS		255

#define FEAT_MOTOR_PRESCALER		64
#define FEAT_MOTOR_PRESCALER_BITS	(1<<CS21) | (1<<CS10)
#define FEAT_MOTOR_TIMER_TICKS		255

#define UART_BAUDRATE				115200

#define RECEIVER_BUFF_SIZE 			128

#define CONTROLLER_TIMEOUT_MS		1000

//Commands
const char* COMMAND_PING 						= "pp";

const char* COMMAND_SERVO_PERCENT 				= "sp";

const char* COMMAND_MOTOR_SPEED 				= "ms";
const char* COMMAND_MOTOR_DIRECTION				= "md";

const char* COMMAND_STATUS_COLOR				= "sc";

const char* COMMAND_SET_FRONT_HEADLIGHT_LEFT	= "sfhl";
const char* COMMAND_SET_FRONT_HEADLIGHT_RIGHT	= "sfhr";
const char* COMMAND_SET_FRONT_HEADLIGHT			= "sfh";
const char* COMMAND_SET_REAR_LIGHT_LEFT			= "srll";
const char* COMMAND_SET_REAR_LIGHT_RIGHT		= "srlr";
const char* COMMAND_SET_REAR_LIGHT				= "srl";
const char* COMMAND_SET_FRONT_FOGLIGHT_LEFT		= "sffl";
const char* COMMAND_SET_FRONT_FOGLIGHT_RIGHT	= "sffr";
const char* COMMAND_SET_FRONT_FOGLIGHT			= "sff";
const char* COMMAND_SET_REVERSING_LIGHT_LEFT	= "srell";
const char* COMMAND_SET_REVERSING_LIGHT_RIGHT	= "srelr";
const char* COMMAND_SET_REVERSING_LIGHT			= "srel";

const char* COMMAND_FEAT1_MOTOR_SPEED 			= "fm1s";
const char* COMMAND_FEAT1_MOTOR_DIRECTION		= "fm1d";
const char* COMMAND_FEAT1_MOTOR_TIMEOUT_S		= "fm1ts";
const char* COMMAND_FEAT1_MOTOR_TIMEOUT_MS		= "fm1tms";

const char* COMMAND_FEAT2_MOTOR_SPEED 			= "fm2s";
const char* COMMAND_FEAT2_MOTOR_DIRECTION		= "fm2d";
const char* COMMAND_FEAT2_MOTOR_TIMEOUT_S		= "fm2ts";
const char* COMMAND_FEAT2_MOTOR_TIMEOUT_MS		= "fm2tms";


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
TimeoutMotor s_featureMotor1;
TimeoutMotor s_featureMotor2;
RgbLed s_stateLed;
Pin s_ledFrontHeadlightLeft;
Pin s_ledFrontHeadlightRight;
Pin s_ledRearLightLeft;
Pin s_ledRearLightRight;
Pin s_ledFrontFoglightLeft;
Pin s_ledFrontFoglightRight;
Pin s_ledReversingLightLeft;
Pin s_ledReversingLightRight;

volatile float s_millisecondsSinceLastControl = 0;
uint8_t s_controllerTimeoutReached = 0;

uint8_t checkControllerTimeoutReached() {
	if(s_controllerTimeoutReached) {
		return s_controllerTimeoutReached;
	}
	if(s_millisecondsSinceLastControl > CONTROLLER_TIMEOUT_MS) {
		s_controllerTimeoutReached = 1;
	}
	return s_controllerTimeoutReached;
}

void resetControllerTimeout() {
	s_millisecondsSinceLastControl = 0;
	s_controllerTimeoutReached = 0;
}

void increaseControllerTimeout(float ms) {
	s_millisecondsSinceLastControl += ms;
}

ISR( TIMER0_OVF_vect )
{
	Clock_tick(&s_clock);
	increaseControllerTimeout(s_clock.tickDurationMs);
	TimeoutMotor_tick(&s_featureMotor1);
	TimeoutMotor_tick(&s_featureMotor2);
}

void initMotorTimer() {
	TCCR1A |= (1 << COM1A0) | (1 << COM1A1); //Timer1A: inverting PWM
	TCCR1A |= (1 << COM1B0) | (1 << COM1B1); //Timer1B: inverting PWM 
	TCCR1A |= (1 << WGM10); //Timer1: Phase correct PWM
	TCCR1B |= MAIN_MOTOR_PRESCALER_BITS; 
	
	DDRD |= (1<<PD5); 						// PD5=OC1A is output
	DDRD |= (1<<PD4); 						// PD4=OC1B is output
	
	TCCR2 |= (1 << COM20) | (1 << COM21); //Timer1A: inverting PWM
	TCCR2 |= (1 << WGM20); //Timer2: Phase correct PWM
	TCCR2 |= FEAT_MOTOR_PRESCALER_BITS; 

	DDRD |= (1<<PD7); 						// PD7=OC2 is output	
}

void initMainMotor() {
	Pin pinDirection;
	Pin_init(&pinDirection, &DDRD, &PORTD, PD6);

	Motor_init(&s_mainMotor, pinDirection, MAIN_MOTOR_TIMER_TICKS, &OCR1A, 0, 1);
}

void initFeatureMotor1() {
	Pin pinDirection;
	Pin_init(&pinDirection, &DDRD, &PORTD, PD3);

	TimeoutMotor_init(&s_featureMotor1, SERVO_TIMER_PERIOD_MS, pinDirection, MAIN_MOTOR_TIMER_TICKS, &OCR1B, 0, 1);
}

void initFeatureMotor2() {
	Pin pinDirection;
	Pin_init(&pinDirection, &DDRC, &PORTC, PC0);

	TimeoutMotor_init(&s_featureMotor2, SERVO_TIMER_PERIOD_MS, pinDirection, FEAT_MOTOR_TIMER_TICKS, 0, &OCR2, 1);
}

void initStatusLed() {
	Pin pinRed;
	Pin_init(&pinRed, &DDRA, &PORTA, PA0);
	Pin pinGreen;
	Pin_init(&pinGreen, &DDRA, &PORTA, PA1);
	Pin pinBlue;
	Pin_init(&pinBlue, &DDRA, &PORTA, PA2);
	RgbLed_init(&s_stateLed, pinRed, pinGreen, pinBlue);
}

void initLEDs() {
	Pin_init(&s_ledFrontHeadlightLeft, &DDRA, &PORTA, PA3);
	Pin_setMode(&s_ledFrontHeadlightLeft, 1);
	Pin_init(&s_ledFrontHeadlightRight, &DDRA, &PORTA, PA4);
	Pin_setMode(&s_ledFrontHeadlightRight, 1);
	Pin_init(&s_ledRearLightLeft, &DDRA, &PORTA, PA5);
	Pin_setMode(&s_ledRearLightLeft, 1);
	Pin_init(&s_ledRearLightRight, &DDRA, &PORTA, PA6);
	Pin_setMode(&s_ledRearLightRight, 1);
	
	Pin_init(&s_ledFrontFoglightLeft, &DDRC, &PORTC, PC7);
	Pin_setMode(&s_ledFrontFoglightLeft, 1);
	Pin_init(&s_ledFrontFoglightRight, &DDRC, &PORTC, PC6);
	Pin_setMode(&s_ledFrontFoglightRight, 1);
	Pin_init(&s_ledReversingLightLeft, &DDRB, &PORTB, PB0);
	Pin_setMode(&s_ledReversingLightLeft, 1);
	Pin_init(&s_ledReversingLightRight, &DDRB, &PORTB, PB1);
	Pin_setMode(&s_ledReversingLightRight, 1);
}

void nextTestStep() {
	PORTB ^= (1<<PB2); 					// toggle PB2		
}

char s_uartRcvBuffer[RECEIVER_BUFF_SIZE + 1];
uint8_t s_uartRcvBufferPos = 0;
uint8_t s_backslashReceived = 0;
uint8_t s_inCommand = 0;

uint8_t handleCommand(char* command, char* value) {
	uint8_t result = 0;
	if(strcmp(command, COMMAND_PING) == 0) {
		result = 1;
	} else if(strcmp(command, COMMAND_MOTOR_SPEED) == 0) {
		double doubleVal = atof(value);
		Motor_setSpeedPercent(&s_mainMotor, (float)doubleVal);
		result = 1;
	} else if(strcmp(command, COMMAND_MOTOR_DIRECTION) == 0) {
		int intVal = atoi(value);
		Motor_setDirection(&s_mainMotor, intVal != 0 ? 1 : 0);
		result = 1;
	} else if(strcmp(command, COMMAND_SERVO_PERCENT) == 0) {
		int intVal = atoi(value);
		Servo_setPercent(intVal);
		result = 1;
	} else if(strcmp(command, COMMAND_STATUS_COLOR) == 0) {
		uint32_t color = strtoul(value, (char**)0, 0);
		RgbLed_setColor(&s_stateLed, color);
		result = 1;
	} else if(strcmp(command, COMMAND_SET_FRONT_HEADLIGHT_LEFT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledFrontHeadlightLeft, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_FRONT_HEADLIGHT_RIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledFrontHeadlightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_FRONT_HEADLIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledFrontHeadlightLeft, on == 0 ? 0 : 1);
		Pin_setValue(&s_ledFrontHeadlightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_REAR_LIGHT_LEFT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledRearLightLeft, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_REAR_LIGHT_RIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledRearLightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_REAR_LIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledRearLightLeft, on == 0 ? 0 : 1);
		Pin_setValue(&s_ledRearLightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_FRONT_FOGLIGHT_LEFT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledFrontFoglightLeft, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_FRONT_FOGLIGHT_RIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledFrontFoglightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_FRONT_FOGLIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledFrontFoglightLeft, on == 0 ? 0 : 1);
		Pin_setValue(&s_ledFrontFoglightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_REVERSING_LIGHT_LEFT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledReversingLightLeft, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_REVERSING_LIGHT_RIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledReversingLightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_SET_REVERSING_LIGHT) == 0) {
		int on = atoi(value) != 0;
		Pin_setValue(&s_ledReversingLightLeft, on == 0 ? 0 : 1);
		Pin_setValue(&s_ledReversingLightRight, on == 0 ? 0 : 1);
		result = 1; 
	} else if(strcmp(command, COMMAND_FEAT1_MOTOR_SPEED) == 0) {
		double doubleVal = atof(value);
		TimeoutMotor_setSpeedPercent(&s_featureMotor1, (float)doubleVal);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT1_MOTOR_DIRECTION) == 0) {
		int intVal = atoi(value);
		TimeoutMotor_setDirection(&s_featureMotor1, intVal != 0 ? 1 : 0);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT1_MOTOR_TIMEOUT_S) == 0) {
		int intVal = atoi(value);
		TimeoutMotor_setRemaining(&s_featureMotor1, intVal * 1000);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT1_MOTOR_TIMEOUT_MS) == 0) {
		int intVal = atoi(value);
		TimeoutMotor_setRemaining(&s_featureMotor1, intVal);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT2_MOTOR_SPEED) == 0) {
		double doubleVal = atof(value);
		TimeoutMotor_setSpeedPercent(&s_featureMotor2, (float)doubleVal);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT2_MOTOR_DIRECTION) == 0) {
		int intVal = atoi(value);
		TimeoutMotor_setDirection(&s_featureMotor2, intVal != 0 ? 1 : 0);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT2_MOTOR_TIMEOUT_S) == 0) {
		int intVal = atoi(value);
		TimeoutMotor_setRemaining(&s_featureMotor2, intVal * 1000);
		result = 1;
	} else if(strcmp(command, COMMAND_FEAT2_MOTOR_TIMEOUT_MS) == 0) {
		int intVal = atoi(value);
		TimeoutMotor_setRemaining(&s_featureMotor2, intVal);
		result = 1;
	}
	return result;
}

uint8_t handleCommands() {
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
	uart_puts("\r\n\\r");
	uart_puts("nc=");
	char buff[12];
	itoa(numOfCommands, buff, 10);
	buff[11] = '\0';
	uart_puts(buff);
	uart_puts("/\r\n");
	
	s_inCommand = 0;
	s_uartRcvBufferPos = 0;
	
	return numOfCommands;
}

void cancelCommand() {
	s_inCommand = 0;
	s_uartRcvBufferPos = 0;
}

uint8_t handlePotentialCommand(char c) {
	if(!s_backslashReceived 
		&& !s_inCommand
		&& s_uartRcvBufferPos == 0) {
		//wait for the beginning
		if(c != '\\') {
			return 0;
		}
		s_backslashReceived = 1;
		return 0;
	}
	if(s_backslashReceived) {
		s_backslashReceived = 0;
		if(c != 'c' && c != 'C') {
			return 0;
		}
		s_inCommand = 1;
		return 0;
	}
	if(s_inCommand) {
		if(c == '/') {
			uint8_t numCmds = handleCommands();
			return numCmds;
		}
		s_uartRcvBuffer[s_uartRcvBufferPos] = c;
		s_uartRcvBufferPos++;
		if(s_uartRcvBufferPos >= RECEIVER_BUFF_SIZE) {
			cancelCommand();
		}
	}
	return 0;
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
			uart_putc(c);
			if(handlePotentialCommand(c)) {
				resetControllerTimeout();
			}
		}
	} while(resultErrorCode == 0);
}

void handleControllerTimeout() {
	Motor_setSpeedPercent(&s_mainMotor, 0);
	Servo_setPercent(50);
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
	DDRB |= (1<<PB2); 						// PC1 is output
	DDRC |= (1<<PC2); 						// PC2 is output
	
	PORTC |= (1 << PC2);
	
	Clock_init(&s_clock, SERVO_TIMER_PERIOD_MS);
	
	Servo_init();
	
	initMotorTimer();
	initMainMotor();
	initFeatureMotor1();
	initFeatureMotor2();
	initStatusLed();
	initLEDs();
	
	RgbLed_setColor(&s_stateLed, 0xFF0000); //Red
	
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
		RgbLed_tick(&s_stateLed);
		
		if(checkControllerTimeoutReached()) {
			handleControllerTimeout();			
		}
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
		wdt_reset();
	}
	return 0; 								// this line should never be reached
}

ISR(BADISR_vect)
{

    for (;;) uart_puts("!");
}