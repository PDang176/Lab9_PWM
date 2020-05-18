/*	Author: Patrick Dang
 *	Partner(s) Name: 
 *	Lab Section: 028
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Video Link: https://drive.google.com/open?id=18l9gvuHjOLMCiot0uBdT6tXnL7rBc2Uz
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{Start, Silent, C, D, E}state;

void Tick(){
	//Transitions
	switch(state){
		case Start:
			state = Silent;
			break;
		case Silent:
		case C:
		case D:
		case E:
			if((~PINA & 0x01) && (PINA & 0x02) && (PINA & 0x04)){
				state = C;
			}
			else if((PINA & 0x01) && (~PINA & 0x02) && (PINA & 0x04)){
                                state = D;
                        }
			else if((PINA & 0x01) && (PINA & 0x02) && (~PINA & 0x04)){
                                state = E;
                        }
			else{
				state = Silent;
			}
			break;
		default:
			state = Start;
			break;	
	}

	//State actions
	switch(state){
		case Start:
			break;
		case Silent:
			set_PWM(0);
			break;
		case C:
			set_PWM(261.63);
			break;
		case D:
			set_PWM(293.66);
			break;
		case E:
			set_PWM(329.63);
			break;
		default:
			set_PWM(0);
			break;
	}
}

void set_PWM(double frequency){
	static double current_frequency;
	if(frequency != current_frequency){
		if(!frequency){ TCCR3B &= 0x08; }
		else{ TCCR3B |= 0x03; }

		if(frequency < 0.954){ OCR3A = 0xFFFF; }
		else if(frequency > 31250){ OCR3A = 0x0000; }
		else{ OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x40; PORTB = 0xBF;

	PWM_on();
    /* Insert your solution below */
    while (1) {
	Tick();
    }
    return 1;
}
