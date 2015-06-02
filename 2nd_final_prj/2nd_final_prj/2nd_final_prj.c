#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task 
{
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;
//--------End Task scheduler data structure-----------------------------------

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char var = 0;
unsigned char var2 = 0;
unsigned char x;
unsigned char global_g = 0;
unsigned char time;


void TimerOn() 
{
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02; 
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80; 
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) 
{
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) 
	{
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) 
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}



enum SM1_States { SM1_off, SM1_on1, SM1_on2, SM1_on3, SM1_on4} SM1_State;

void TickFct_State_machine_1() 
{
	switch(SM1_State) 
	{

		case SM1_off:
		if(global_g == 0)
		{
			if(~PINB & 0x10)
			{
				SM1_State = SM1_on1;
			}
			else
			{
				SM1_State = SM1_off;
			}
		}
		break;
		case SM1_on1:
		SM1_State = SM1_on2;
		break;
		case SM1_on2:
		SM1_State = SM1_on3;
		break;
		case SM1_on3:
		SM1_State = SM1_on4;
		break;
		case SM1_on4:
		SM1_State = SM1_off;
		global_g = 1;
	}

	switch(SM1_State) 
	{
		case SM1_off:
		PORTA = 0X00;
		break;
		case SM1_on1:
		PORTA = 0x01;
		break;
		case SM1_on2:
		PORTA = 0x02;
		break;
		case SM1_on3:
		PORTA = 0x04;
		break;
		case SM1_on4:
		PORTA = 0x08;
		break;
		default:
		break;
	}

}

enum Press_States {Init, Press1, Press2, Press3, Press4, Error, Off} Press_States;
void ButtonPress()
{
	switch(Press_States)
	{
		case Init:
		if(global_g == 1)
		{
			if(~PINB & 0x01)
			{
				while(~PINB & 0x01){}
				Press_States = Press1;	
			}
			if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
			{
				Press_States = Error;
			}
		}
		break;
		
		case Press1:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_States = Press2;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_States = Error;
		}
		break;
		case Press2:
		if(~PINB & 0x04)
		{
			while(~PINB & 0x04){}
			Press_States = Press3;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_States = Error;
		}
		break;
		case Press3:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_States = Press4;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_States = Error;
		}
		break;
		case Press4:
		if(~PINB & 0x10)
		{
			Press_States = Off;	
		}
		break;
		case Error:
		if(~PINB & 0x01)
		{
			Press_States = Press1;
		}
		break;
	}
	switch(Press_States)
	{
		case Error:
		PORTA = 0x10;
		break;
		
		case Press1:
		PORTA = 0x01;
		break;
		
		case Press2:
		PORTA = 0x02;
		break;
		
		case Press3:
		PORTA = 0x04;
		break;
		
		case Press4:
		PORTA = 0x08;
		break;
		case Off:
		PORTA = 0x00;
		global_g = 2;
		default:
		break;
		
	}
}

enum SM2_State {SM2_off, SM2_on1, SM2_on2, SM2_on3, SM2_on4, SM2_on5, SM2_on6} SM2_States;
void TickFct_Machine2()
{
	switch(SM2_States)
	{
		case SM2_off:
		if(global_g == 2)
		{
			if(~PINB & 0x01)
			{
				SM2_States = SM2_on1;
			}
			while(!(~PINB & 0x01))
			{
				SM2_States = SM2_off;
			}	
		}
		else{
			SM2_States = SM2_off;
		}
		break;
		case SM2_on1:
		SM2_States = SM2_on2;
		break;
		case SM2_on2:
		SM2_States = SM2_on3;
		break;
		case SM2_on3:
		SM2_States = SM2_on4;
		break;
		case SM2_on4:
		SM2_States = SM2_on5;
		break;
		case SM2_on5:
		SM2_States = SM2_on6;
		break;
		case SM2_on6:
		global_g = 3;
		SM2_States = SM2_off;
		break;
	}

	switch(SM2_States)
	{
		case SM2_off:
		PORTA = 0x00;
		break;
		case SM2_on1:
		PORTA = 0x01;
		break;
		case SM2_on2:
		PORTA = 0x04;
		break;
		case SM2_on3:
		PORTA = 0x08;
		break;
		case SM2_on4:
		PORTA = 0x01;
		break;
		case SM2_on5:
		PORTA = 0x02;
		break;
		case SM2_on6:
		PORTA = 0x01;
		break;
	}
}

enum Press_sec_level{Init2, Press_state1, Press_state2, Press_state3, Press_state4, Press_state5, Press_state6, Error2, Off2} Press_sec_level;
void ButtonPress2()
{
	switch(Press_sec_level)
	{
		case Init2:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_sec_level = Press_state1;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_sec_level = Error2;
		}
		break;
		case Press_state1:
		if(~PINB & 0x04)
		{
			while(~PINB & 0x04){}
			Press_sec_level = Press_state2;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_sec_level = Error;
		}
		break;

		case Press_state2:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_sec_level = Press_state3;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_sec_level = Error2;
		}
		break;

		case Press_state3:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_sec_level = Press_state4;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_sec_level = Error2;
		}
		break;

		case Press_state4:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_sec_level = Press_state5;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_sec_level = Error2;
		}
		break;

		case Press_state5:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_sec_level = Press_state6;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_sec_level = Error2;
		}
		break;

		case Press_state6:
		if(~PINB & 0x10)
		{
			Press_sec_level = Off2;
		}
		break;
		default:
		Press_sec_level = Init2;
	}
	switch(Press_sec_level)
	{
		case Error2:
		PORTA = 0x10;
		break;

		case Press_state1:
		PORTA = 0x01;
		var2++;
		break;

		case Press_state2:
		PORTA = 0x04;
		var2++;
		break;

		case Press_state3:
		PORTA = 0x08;
		var2++;
		break;

		case Press_state4:
		PORTA = 0x01;
		var2++;
		break;

		case Press_state5:
		PORTA = 0x02;
		var2++;
		break;

		case Press_state6:
		PORTA = 0x01;
		var2++;
		break;

		case Off2:
		PORTA = 0x00;
		global_g = 4;
		default:
		break;

	}
}

enum SM3_States {SM3_off, SM3_on1, SM3_on2, SM3_on3, SM3_on4, SM3_on5, SM3_on6, SM3_on7, SM3_on8, SM3_on9} SM3_State;
void TickFct_Machine3()
{
	switch(SM3_States)
	{
		case SM3_off:
		if(global_g == 4)
		{
			if(~PINB & 0x01)
			{
				while(~PINB & 0x01){}
				SM3_State = SM3_on1;
			}
			else
			{
				SM3_State = SM3_off;
			}	
		}
		break;
		case SM3_on1:
		SM3_State = SM3_on2;
		break;
		case SM3_on2:
		SM3_State = SM3_on3;
		break;
		case SM3_on3:
		SM3_State = SM3_on4;
		break;
		case SM3_on4:
		SM3_State = SM3_on5;
		break;
		case SM3_on5:
		SM3_State = SM3_on6;
		break;
		case SM3_on6:
		SM3_State = SM3_on7;
		break;
		case SM3_on7:
		SM3_State = SM3_on8;
		break;
		case SM3_on8:
		SM3_State = SM3_on9;
		break;
		case SM3_on9:
		SM3_State = SM3_off;
		default:
		SM3_State = SM3_off;

	}
}

int main(void)
{
	DDRA = 0xff;	PORTA = 0x00;
	DDRB = 0x00;	PORTB = 0xFF;
	
	if(global_g == 0)
	{
		while(global_g == 0)
		{
			TimerSet(700);
			TimerOn();
			TickFct_State_machine_1();
			while(!TimerFlag);
			TimerFlag = 0;	
		}
		while(global_g == 1)
		{
			TimerSet(200);
			TimerOn();
			ButtonPress();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 2)
		{
			TimerSet(1000);
			TimerOn();
			TickFct_Machine2();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 3)
		{
			TimerSet(200);
			TimerOn();
			ButtonPress2();
			while(!TimerFlag);
			TimerFlag = 0;
		}
	}
	return 0;
		
}