/*	Author: Patrick Dang
 *	Partner(s) Name: 
 *	Lab Section: 028
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Video Link: https://drive.google.com/open?id=18s0c9Spla3w74atdUa7crGtu-UJiiS6_
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

const double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
const unsigned char melody[20] = {0, 1, 4, 3, 2, 2, 5, 7, 3, 2, 2, 2, 3, 6, 6, 6, 1, 0, 0, 2};
unsigned char i;
unsigned char j;

enum States{Start, Wait_Press, Play_Melody, Wait_Release}state;

void Tick(){
	//Transitions
	switch(state){
		case Start:
			state = Wait_Press;
			break;
		case Wait_Press:
			if(~PINA & 0x01){
				state = Play_Melody;
				j = 0;
				i = 0;
			}
			else{
				state = Wait_Press;
			}
			break;
		case Play_Melody:
			if(j <= 20){
				state = Play_Melody;
				i++;
			}
			else{
				state = (~PINA & 0x01) ? Wait_Release : Wait_Press;
			}
			break;
		case Wait_Release:
			state = (~PINA & 0x01) ? Wait_Release : Wait_Press;
			break;
		default:
			state = Start;
			break;
	}
	//State actions
	switch(state){
		case Start:
			break;
		case Wait_Press:
			set_PWM(0);
			break;
		case Play_Melody:
			j++;
			set_PWM(notes[melody[i]]);
			break;	
		case Wait_Release:
			set_PWM(0);
			break;
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
	TimerSet(250);
	TimerOn();

	state = Start;
    /* Insert your solution below */
    while (1) {
	Tick();
	while(!TimerFlag){}
	TimerFlag = 0;
    }
    return 1;
}
