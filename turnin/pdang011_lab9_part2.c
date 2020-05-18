/*	Author: Patrick Dang
 *	Partner(s) Name: 
 *	Lab Section: 028
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Video Link: https://drive.google.com/open?id=18mqLLv6RcBwiFmZe7x2BBFItEOPsdV81
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

const double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char i;

enum Toggle_States{TStart, OFF, OFF_HOLD, ON, ON_HOLD}toggle_state;

void Toggle_Tick(){
	//Transitions
	switch(toggle_state){
		case TStart:
			toggle_state = OFF;
			break;
		case OFF:
			toggle_state = (~PINA & 0x01) ? ON_HOLD : OFF;
			break;
		case OFF_HOLD:
			toggle_state = (~PINA & 0x01) ? OFF_HOLD : OFF;
			break;
		case ON:
			toggle_state = (~PINA & 0x01) ? OFF_HOLD : ON;
			break;
		case ON_HOLD:
			toggle_state = (~PINA & 0x01) ? ON_HOLD : ON;
			break;
		default:
			toggle_state = TStart;
			break;
	}
	//State actions
	switch(toggle_state){
		case TStart:
			i = 0;
			break;
		case OFF:
		case OFF_HOLD:
			set_PWM(0);
			break;
		case ON:
		case ON_HOLD:
			set_PWM(notes[i]);
			break;
		default:
			break;
	}
}

enum Change_States{CStart, Wait_Press, Increase_HOLD, Decrease_HOLD}change_state;

void Change_Tick(){
	//Transitions
	switch(change_state){
                case CStart:
			change_state = Wait_Press;
			break;
                case Wait_Press:
			if((~PINA & 0x02) && (PINA & 0x04)){
				change_state = Increase_HOLD;
				i = (i < 7) ? i + 1 : i;
			}
			else if((PINA & 0x02) && (~PINA & 0x04)){
				change_state = Decrease_HOLD;
				i = (i > 0) ? i - 1 : i;
			}
			else{
				change_state = Wait_Press;
			}
			break;
                case Increase_HOLD:
			change_state = (~PINA & 0x02) ? Increase_HOLD : Wait_Press;
			break;
                case Decrease_HOLD:
			change_state = (~PINA & 0x04) ? Decrease_HOLD : Wait_Press;
			break;
                default:
			change_state = CStart;
			break;
        }

	//State actions
	switch(change_state){
		case CStart:
			i = 0;
			break;
		case Wait_Press:
		case Increase_HOLD:
		case Decrease_HOLD:
		default:
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
	TimerSet(50);
	TimerOn();

	toggle_state = TStart;
	change_state = CStart;
    /* Insert your solution below */
    while (1) {
	Toggle_Tick();
	Change_Tick();
	while(!TimerFlag){}
	TimerFlag = 0;
    }
    return 1;
}
