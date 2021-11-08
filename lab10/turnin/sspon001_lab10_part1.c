/*	Author: sspon001
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum ThreeLEDsSM {threeStart, one, two, three} ThreeLEDstate ;
enum BlinkingLEDSM {blinkStart, blinkInit, blink} BlinkingLEDstate ;
enum CombineLEDsSM {combineStart, combineInit} CombineLEDstate ;
unsigned char threeLEDs = 0x00 ;
unsigned char blinkLED = 0x00 ;

volatile unsigned char TimerFlag = 0 ;
unsigned long _avr_timer_M = 1 ;
unsigned long _avr_timer_cntcurr = 0 ;
void TimerISR() { TimerFlag = 1 ;}
void TimerOn() {
	TCCR1B = 0x0B ;
	OCR1A = 125 ;
	TIMSK1 = 0x02 ;
	TCNT1 = 0 ;
	_avr_timer_cntcurr = _avr_timer_M ;
	SREG |= 0x80 ;
}
void TimerOff() {
	TCCR1B = 0x00 ;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr-- ;
	if (_avr_timer_cntcurr == 0) {
		TimerISR() ;
		_avr_timer_cntcurr = _avr_timer_M ;
	}
}
void TimerSet (unsigned long M) {
	_avr_timer_M = M ;
	_avr_timer_cntcurr = _avr_timer_M ;
}
void TickThreeLEDsSM(){
	switch(ThreeLEDstate){
		case threeStart:
			ThreeLEDstate = one ;
			break ;
		case one:
			ThreeLEDstate = two ;
			break ;
		case two:
			ThreeLEDstate = three ;
			break ;
		case three:
			ThreeLEDstate = one ;
			break ;
		default:
			ThreeLEDstate = threeStart ;
			break ;
	}
	switch(ThreeLEDstate){
		case threeStart:
			break ;
		case one: 
			threeLEDs = 0x01 ;
			break ;
		case two:
			threeLEDs = 0x02 ;
			break ;
		case three:
			threeLEDs = 0x04 ;
			break ;
		default:
			break ;
	}
}
void TickBlinkingLEDSM(){
	switch(BlinkingLEDstate){
		case blinkStart:
			BlinkingLEDstate = blinkInit ;
			break ;
		case blinkInit:
			BlinkingLEDstate = blink ;
			break ;
		case blink:
			BlinkingLEDstate = blinkInit ;
			break ;
		default:
			BlinkingLEDstate = blinkStart ;
			break ;
	}
	switch(BlinkingLEDstate){
		case blinkStart:
			break ;
		case blinkInit:
			blinkLED = 0x00 ;
			break ;
		case blink:
			blinkLED = 0x01 ;
			break ;
		default:
			break ;
	}
}
void TickCombineLEDsSM(){
	switch(CombineLEDstate){
		case combineStart:
			CombineLEDstate = combineInit ;
			break ;
		case combineInit:
			CombineLEDstate = combineInit ;
			break ;
		default:
			CombineLEDstate = combineStart ;
			break ;
	}
	switch(CombineLEDstate){
		case combineStart:
			break ;
		case combineInit:
			PORTB = ((blinkLED << 3) | threeLEDs) ;
			break ;
		default:
			break ;
	}
}

int main(void) {
	DDRA = 0x00 ; PORTA = 0xFF ;
	DDRB = 0xFF ; PORTB = 0x00 ;
	TimerSet(1000) ;
	TimerOn() ;
    while (1) {
	    TickThreeLEDsSM() ;
	    TickBlinkingLEDSM() ;
	    TickCombineLEDsSM() ;
	    while(!TimerFlag) {} ;
	    TimerFlag = 0 ;
    }
    return 1;
}
