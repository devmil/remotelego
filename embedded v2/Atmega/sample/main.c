#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <util/delay.h>

#define SERVO_PRESCALER 		1024
#define SERVO_PRESCALER_BITS	(1<<CS00) | (1<<CS02)
#define SERVO_TIMER_TICKS		255
#define SERVO_EFFECTIVE_F		(F_CPU / SERVO_PRESCALER) //14745000 / 1024 = 14399
#define SERVO_TIMER_PERIOD_MS	(((float)1000 * SERVO_TIMER_TICKS) / SERVO_EFFECTIVE_F)	//(1000 * 255) / 14399 = 17,70956316410862
#define SERVO_TIMER_MS_PER_TICK	(SERVO_TIMER_PERIOD_MS / SERVO_TIMER_TICKS) //0,06944926731023
#define SERVO_MIN_MS			0.8f
#define SERVO_MAX_MS			2.3f

void setServoPercent(float percent) {
	float servoMs = SERVO_MIN_MS + ((SERVO_MAX_MS - SERVO_MIN_MS) * percent) / 100;
	float servoTicks = servoMs / SERVO_TIMER_MS_PER_TICK;
	OCR0 = 255 - (int8_t)servoTicks;
}

void initServo() {
	DDRB |= (1<<PB3); 						// PB3 == OC0 is servo output
	PORTB |= (1<<PB3);						// pull down

	TCCR0 |= (1<<WGM00) | (1<<WGM01);		// fast PWM mode
	TCCR0 |= (1<<COM01);					// compare output mode
	TCCR0 |= (1<<COM00);					// non inverted PWM
	TCCR0 |= SERVO_PRESCALER_BITS;

	setServoPercent(50);					//center servo	
}

int main(void)
{	
	DDRD |= (1<<PD7); 						// PD7 is output
	
	initServo();

	int8_t direction = 1;
	int8_t waitingCycles = 0;
	int8_t isWaiting = 0;
	float servo_percent = 50;
	
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
			setServoPercent(servo_percent);
		} else {
			waitingCycles--;
		}
	}
	return 0; 								// this line should never be reached
}

// #define PRESCALER      		  1
// #define PRESCALER_BITS  	  (1<<CS20)
// #define TICKS_PER_SECOND      ( F_CPU / PRESCALER / 256 ) //57598
// #define TICKS_PER_MILLISECOND ( TICKS_PER_SECOND / 1000 ) //58

// typedef struct {
// 	uint16_t counter;
// 	uint16_t periodTimerTicks;
// 	uint16_t minTimerTicks;
// 	uint16_t maxTimerTicks;
// 	uint16_t endTimerTicks;
// } Servo;

// Servo servo1;

// void onServoTick(Servo* servo) {
// 	servo->counter = servo->counter + 1;
// 	if(servo->periodTimerTicks <= servo->counter) {
// 		servo->counter = 0;
// 		PORTD |= (1<<PD7);
// 	}
// 	if(servo->counter == servo->endTimerTicks) {
// 		PORTD &= ~(1<<PD7);
// 	}
// }

// void setServoPercent(Servo* servo, uint8_t percent) {
// 	servo->endTimerTicks = servo->minTimerTicks + (((servo->maxTimerTicks - servo->minTimerTicks) * percent) / 100);
// }

// void initServo(Servo* servo, float periodMs, float minMs, float maxMs) {
// 	servo->counter = 0;
// 	servo->periodTimerTicks = (uint16_t)(TICKS_PER_MILLISECOND * periodMs);
// 	servo->minTimerTicks = (uint16_t)(TICKS_PER_MILLISECOND * minMs);
// 	servo->maxTimerTicks = (uint16_t)(TICKS_PER_MILLISECOND * maxMs);
// 	setServoPercent(servo, 50);
// }

// void onTick() {
// 	onServoTick(&servo1);
// }

// ISR( TIMER2_OVF_vect )
// {
// 	onTick();
// }

// int main(void)
// {	
// 	DDRD |= (1<<PD7); 						// PD7 is output
// 	PORTD = (1<<PD7);
	
// 	initServo(&servo1, 20.0f, 0.75f, 2.3f);
	
// 	TCCR2 = PRESCALER_BITS;
		
// 	TIMSK |= (1<<TOIE2); //Overflow interrupt
	
// 	sei(); 									// enable interrupts
	
// 	int8_t direction = 1;
// 	int8_t waitingCycles = 0;
// 	int8_t isWaiting = 0;
// 	int8_t servo_percent = 50;
	
// 	while (42)
// 	{
// 		_delay_ms(500); 					// wait some time
		
// 		if(!isWaiting && servo_percent == 50) {
// 			if(waitingCycles == 0) {
// 				isWaiting = 1;
// 				waitingCycles = 6;
// 			}
// 		}
// 		if(waitingCycles == 0) {
// 			isWaiting = 0;
// 			servo_percent += direction * 10;
// 			if(servo_percent > 100) {
// 				direction *= -1;
// 				servo_percent = 100;
// 			} else if(servo_percent < 0) {
// 				direction *= -1;
// 				servo_percent = 0;
// 			}
// 			cli(); 								// disable interrupts
// 			setServoPercent(&servo1, servo_percent);
// 			sei(); 								// enable interrupts
// 		} else {
// 			waitingCycles--;
// 		}
		
		
// 	}
// 	return 0; 								// this line should never be reached
// }


/*
#define TICKS_PERIOD		  ( F_CPU / PRESCALER / 50 ) //20ms
#define TICKS_MILLISEC_BASE   ( TICKS_PERIOD / 20 ) 
#define TICKS_CENTER          ( TICKS_MILLISEC_BASE / 2 )
#define TICKS_MILLISEC_MIN    ( TICKS_PERIOD / 25 ) //0,8ms
#define TICKS_MILLISEC_CENTER ( TICKS_PERIOD / 13 ) //1,5ms
#define TICKS_MILLISEC_MAX    ( TICKS_PERIOD / 9 ) //2,22ms
#define TICKS_MILLISEC_100P   ( TICKS_MILLISEC_MAX - TICKS_MILLISEC_MIN ) 

int8_t servo_percent = 0;
uint8_t pin_state = 0;

ISR( TIMER1_COMPA_vect )
{
	uint16_t active_time_ticks = (((TICKS_MILLISEC_100P * servo_percent) / 100) + TICKS_MILLISEC_MIN);
	
	if(!pin_state) {
		OCR1A = active_time_ticks;
		pin_state = 1;
	} else {
		OCR1A = TICKS_PERIOD - active_time_ticks;		
		pin_state = 0;
	}
  //OCR1A = 2500-OCR1A;
}

int main(void)
{	
	DDRD |= (1<<PD7); 						// PD7 is output
	
	DDRD |= (1<<PD5); 						// PD5 == OC1A is servo output
	
	TCCR1A = (1<<COM1A0); 					// toggle on compare match
	TCCR1B = (1<<WGM12) | PRESCALER_BITS; 	// CTC mode + Prescaler
	TIMSK  = (1<<OCIE1A); 					// enable timer compare interrupt
	
	OCR1A = 1;
	
	sei(); 									// enable interrupts
	
	int8_t direction = 1;
	int8_t waitingCycles = 0;
	int8_t isWaiting = 0;
	
	while (42)
	{
		_delay_ms(500); 					// wait some time
		PORTD ^= (1<<PD7); 					// toggle PD7
		
		cli(); 								// disable interrupts
		
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
		} else {
			waitingCycles--;
		}
		
		sei(); 								// enable interrupts
	}
	return 0; 								// this line should never be reached
}

*/