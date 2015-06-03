#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
unsigned char var = 0;
unsigned char var2 = 0;
unsigned char x;
unsigned char global_g = 0;
unsigned char time;


#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTD		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTA	// port connected to pins 4 and 6 of LCD disp.
#define RS 0			// pin number of uC connected to pin 4 of LCD disp.
#define E 1			// pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
	LCD_WriteCommand(0x01);
}

void LCD_init(void) {

	//wait for 100 ms.
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);
}

void LCD_WriteCommand (unsigned char Command) {
	CLR_BIT(CONTROL_BUS,RS);
	DATA_BUS = Command;
	SET_BIT(CONTROL_BUS,E);
	asm("nop");
	CLR_BIT(CONTROL_BUS,E);
	delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
	SET_BIT(CONTROL_BUS,RS);
	DATA_BUS = Data;
	SET_BIT(CONTROL_BUS,E);
	asm("nop");
	CLR_BIT(CONTROL_BUS,E);
	delay_ms(1);
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
	LCD_ClearScreen();
	unsigned char c = column;
	while(*string) {
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}

void LCD_Cursor(unsigned char column) {
	if ( column < 17 ) { // 16x1 LCD: column < 9
		// 16x2 LCD: column < 17
		LCD_WriteCommand(0x80 + column - 1);
		} else {
		LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
		// 16x2 LCD: column - 9
	}
}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
	int i,j;
	for(i=0;i<miliSec;i++)
	for(j=0;j<775;j++)
	{
		asm("nop");
	}
}


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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//code for the lcd display
void LCD_init();
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void delay_ms(int miliSec);

///////////////////////////////////////////////begin code/////////////////////////////////////////////////////////////////////////////


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
			while(~PINB & 0x01){}
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
		else
		{
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
			Press_sec_level = Error2;
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
		break;

		case Press_state2:
		PORTA = 0x04;
		break;

		case Press_state3:
		PORTA = 0x08;
		break;

		case Press_state4:
		PORTA = 0x01;
		break;

		case Press_state5:
		PORTA = 0x02;
		break;

		case Press_state6:
		PORTA = 0x01;
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
	switch(SM3_State)
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
	}
	switch(SM3_State)
	{
		case SM3_off:
		PORTA = 0x00;
		break;
		case SM3_on1:
		PORTA = 0x02;
		break;
		case SM3_on2:
		PORTA = 0x04;
		break;
		case SM3_on3:
		PORTA = 0x01;
		break;
		case SM3_on4:
		PORTA = 0x08;
		break;
		case SM3_on5:
		PORTA = 0x02;
		break;
		case SM3_on6:
		PORTA = 0x04;
		break;
		case SM3_on7:
		PORTA = 0x01;
		break;
		case SM3_on8:
		PORTA = 0x02;
		break;
		case SM3_on9:
		PORTA = 0x08;
		global_g = 5;
		break;
	}
}

enum Press_third_level{Init3, Press3_state1, Press3_state2, Press3_state3, Press3_state4, Press3_state5, Press3_state6, Press3_state7, Press3_state8, Press3_state9, Error3, Off3} Press_third_level;
void ButtonPress3()
{
	switch(Press_third_level)
	{
		case Init3:
		if(global_g == 5)
		{
			PORTA = 0x00;
			if(~PINB & 0x02)
			{
				while(~PINB & 0x02){}
				Press_third_level = Press3_state1;
			}
			if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
			{
				Press_third_level = Error3;
			}
		}
		break;
		case Error3:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_third_level = Press3_state1;
		}
		break;
		case Press3_state1:
		if(~PINB & 0x04)
		{
			
			while(~PINB & 0x04){}
			Press_third_level = Press3_state2;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state2:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_third_level = Press3_state3;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state3:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_third_level = Press3_state4;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state4:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_third_level = Press3_state5;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state5:
		if(~PINB & 0x04)
		{
			
			while(~PINB & 0x04){}
			Press_third_level = Press3_state6;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state6:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_third_level = Press3_state7;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state7:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_third_level = Press3_state8;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state8:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_third_level = Press3_state9;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_third_level = Error3;
		}
		break;
		case Press3_state9:
		if(~PINB & 0x10)
		{
			Press_third_level = Off3;
		}
		break;
	}
	switch(Press_third_level)
	{
		case Error3:
		PORTA = 0x10;
		break;
		case Press3_state1:
		PORTA = 0x02;
		break;
		case Press3_state2:
		PORTA = 0x04;
		break;
		case Press3_state3:
		PORTA = 0x01;
		break;
		case Press3_state4:
		PORTA = 0x08;
		break;
		case Press3_state5:
		PORTA = 0x02;
		break;
		case Press3_state6:
		PORTA = 0x04;
		break;
		case Press3_state7:
		PORTA = 0x01;
		break;
		case Press3_state8:
		PORTA = 0x02;
		break;
		case Press3_state9:
		PORTA = 0x08;
		break;
		case Off3:
		PORTA = 0x00;
		global_g = 6;
		break;
	}
}

enum SM4_States {SM4_off, SM4_on1, SM4_on2, SM4_on3, SM4_on4, SM4_on5, SM4_on6, SM4_on7, SM4_on8, SM4_on9, SM4_on10, SM4_on11, SM4_on12} SM4_States;
void TickFct_Machine4()
{
	switch(SM4_States)
	{
		case SM4_off:
		if(global_g == 6)
		{
			if(~PINB & 0x01)
			{
				while(~PINB & 0x01){}
				SM4_States = SM4_on1;
			}
			else
			{	
				SM4_States = SM4_off;
			}
		}
		break;
		case SM4_on1:
		SM4_States = SM4_on2;
		break;
		case SM4_on2:
		SM4_States = SM4_on3;
		break;
		case SM4_on3:
		SM4_States = SM4_on4;
		break;
		case SM4_on4:
		SM4_States = SM4_on5;
		break;
		case SM4_on5:
		SM4_States = SM4_on6;
		break;
		case SM4_on6:
		SM4_States = SM4_on7;
		break;
		case SM4_on7:
		SM4_States = SM4_on8;
		break;
		case SM4_on8:
		SM4_States = SM4_on9;
		break;
		case SM4_on9:
		SM4_States = SM4_on10;
		break;
		case SM4_on10:
		SM4_States = SM4_on11;
		break;
		case SM4_on11:
		SM4_States = SM4_on12;
		break;
		case SM4_on12:
		SM4_States = SM4_off;
		break;
	}
	switch(SM4_States)
	{
		case SM4_off:
		PORTA = 0x00;
		break;
		case SM4_on1:
		PORTA = 0x01;
		break;
		case SM4_on2:
		PORTA = 0x08;
		break;
		case SM4_on3:
		PORTA = 0x02;
		break;
		case SM4_on4:
		PORTA = 0x01;
		break;
		case SM4_on5:
		PORTA = 0x02;
		break;
		case SM4_on6:
		PORTA = 0x08;
		break;
		case SM4_on7:
		PORTA = 0x02;
		break;
		case SM4_on8:
		PORTA = 0x01;
		break;
		case SM4_on9:
		PORTA = 0x08;
		break;
		case SM4_on10:
		PORTA = 0x01;
		break;
		case SM4_on11:
		PORTA = 0x02;
		break;
		case SM4_on12:
		PORTA = 0x01;
		global_g = 7;
		break;
	}
}

enum Press_fourth_level{Init4, Press4_state1, Press4_state2, Press4_state3, Press4_state4, Press4_state5, Press4_state6, Press4_state7, Press4_state8, Press4_state9, Press4_state10, Press4_state11, Press4_state12, Error4, Off4} Press_fourth_level;
void ButtonPress4()
{
	switch(Press_fourth_level)
	{
		case Init4:
		if(global_g == 7)
		{
			PORTA = 0x00;
			if(~PINB & 0x01)
			{
				while(~PINB & 0x01){}
				Press_fourth_level = Press4_state1; 
			}
			if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
			{
				Press_fourth_level = Error4;
			}
		}
		break;
		case Press4_state1:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_fourth_level = Press4_state2;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state2:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fourth_level = Press4_state3;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state3:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fourth_level = Press4_state4;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state4:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fourth_level = Press4_state5;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state5:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_fourth_level = Press4_state6;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state6:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fourth_level = Press4_state7;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state7:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fourth_level = Press4_state8;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state8:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_fourth_level = Press4_state9;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state9:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fourth_level = Press4_state10;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state10:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fourth_level = Press4_state11;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		
		case Press4_state11:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fourth_level = Press4_state12;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fourth_level = Error4;
		}
		break;
		case Press4_state12:
		if(~PINB & 0x10)
		{
			Press_fourth_level = Off4;
		}
		break;
		case Error4:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fourth_level = Press4_state1;
		}
	}
	switch(Press_fourth_level)
	{
		case Error4:
		PORTA = 0x10;
		break;
		case Press4_state1:
		PORTA = 0x01;
		break;
		case Press4_state2:
		PORTA = 0x08;
		break;
		case Press4_state3:
		PORTA = 0x02;
		break;
		case Press4_state4:
		PORTA = 0x01;
		break;
		case Press4_state5:
		PORTA = 0x02;
		break;
		case Press4_state6:
		PORTA = 0x08;
		break;
		case Press4_state7:
		PORTA = 0x02;
		break;
		case Press4_state8:
		PORTA = 0x01;
		break;
		case Press4_state9:
		PORTA = 0x08;
		break;
		case Press4_state10:
		PORTA = 0x01;
		break;
		case Press4_state11:
		PORTA = 0x02;
		break;
		case Press4_state12:
		PORTA = 0x01;
		break;
		case Off4:
		PORTA = 0x00;
		global_g = 8;
		break;
	}
}	

enum SM5_States{SM5_off, SM5_on1, SM5_on2, SM5_on3, SM5_on4, SM5_on5, SM5_on6, SM5_on7, SM5_on8, SM5_on9, SM5_on10, SM5_on11, SM5_on12, SM5_on13, SM5_on14, SM5_on15} SM5_States;
void TickFct_Machine5()
{
	switch(SM5_States)
	{
		case SM5_off:
		
		if(global_g == 8)
		{
			if(~PINB & 0x01)
			{
				while(~PINB & 0x01){}
				SM5_States = SM5_on1;
			}
			else
			{
				SM5_States = SM5_off;
			}
		}
		break;
		case SM5_on1:
		SM5_States = SM5_on2;
		break;
		case SM5_on2:
		SM5_States = SM5_on3;
		break;
		case SM5_on3:
		SM5_States = SM5_on4;
		break;
		case SM5_on4:
		SM5_States = SM5_on5;
		break;
		case SM5_on5:
		SM5_States = SM5_on6;
		break;
		case SM5_on6:
		SM5_States = SM5_on7;
		break;
		case SM5_on7:
		SM5_States = SM5_on8;
		break;
		case SM5_on8:
		SM5_States = SM5_on9;
		break;
		case SM5_on9:
		SM5_States = SM5_on10;
		break;
		case SM5_on10:
		SM5_States = SM5_on11;
		break;
		case SM5_on11:
		SM5_States = SM5_on12;
		break;
		case SM5_on12:
		SM5_States = SM5_on13;
		break;
		case SM5_on13:
		SM5_States = SM5_on14;
		break;
		case SM5_on14:
		SM5_States = SM5_on15;
		break;
		case SM5_on15:
		SM5_States = SM5_off;
		break;
	}
	switch(SM5_States)
	{
		case SM5_off:
		PORTA = 0x00;
		break;
		case SM5_on1:
		PORTA = 0x08;
		break;
		case SM5_on2:
		PORTA = 0x04;
		break;
		case SM5_on3:
		PORTA = 0x02;
		break;
		case SM5_on4:
		PORTA = 0x01;
		break;
		case SM5_on5:
		PORTA = 0x02;
		break;
		case SM5_on6:
		PORTA = 0x01;
		break;
		case SM5_on7:
		PORTA = 0x04;
		break;
		case SM5_on8:
		PORTA = 0x08;
		break;
		case SM5_on9:
		PORTA = 0x02;
		break;
		case SM5_on10:
		PORTA = 0x01;
		break;
		case SM5_on11:
		PORTA = 0x02;
		break;
		case SM5_on12:
		PORTA = 0x01;
		break;
		case SM5_on13:
		PORTA = 0x04;
		break;
		case SM5_on14:
		PORTA = 0x08;
		break;
		case SM5_on15:
		PORTA = 0x01;
		global_g = 9;
		break;
	}
}

enum Press_fifth_level{Init5, Press5_state1, Press5_state2, Press5_state3, Press5_state4, Press5_state5, Press5_state6, Press5_state7, Press5_state8, Press5_state9, Press5_state10, Press5_state11, Press5_state12, Press5_state13, Press5_state14, Press5_state15, Error5, Off5} Press_fifth_level;
void ButtonPress5()
{
	switch(Press_fifth_level)
	{
		case Init5:
		if(global_g == 9)
		{
			PORTA = 0x00;
			if(~PINB & 0x08)
			{
				while(~PINB & 0x08){}
				Press_fifth_level = Press5_state1;
			}
			if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
			{
				Press_fifth_level = Error5;
			}
		}
		break;
		case Press5_state1:
		if(~PINB & 0x04)
		{
			while(~PINB & 0x04){}
			Press_fifth_level = Press5_state2;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state2:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fifth_level = Press5_state3;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state3:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fifth_level = Press5_state4;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state4:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fifth_level = Press5_state5;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state5:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fifth_level = Press5_state6;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state6:
		if(~PINB & 0x04)
		{
			
			while(~PINB & 0x04){}
			Press_fifth_level = Press5_state7;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state7:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_fifth_level = Press5_state8;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state8:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fifth_level = Press5_state9;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state9:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fifth_level = Press5_state10;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state10:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_fifth_level = Press5_state11;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state11:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fifth_level = Press5_state12;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state12:
		if(~PINB & 0x04)
		{
			while(~PINB & 0x04){}
			Press_fifth_level = Press5_state13;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state13:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_fifth_level = Press5_state14;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state14:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_fifth_level = Press5_state15;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_fifth_level = Error5;
		}
		break;
		case Press5_state15:
		if(~PINB & 0x10)
		{
			Press_fifth_level = Off5;
		}
		break;
		case Error5:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_fifth_level = Press5_state1;
		}
		break;
	}
	switch(Press_fifth_level)
	{
		case Error5:
		PORTA = 0x10;
		break;
		case Press5_state1:
		PORTA = 0x08;
		break;
		case Press5_state2:
		PORTA = 0x04;
		break;
		case Press5_state3:
		PORTA = 0x02;
		break;
		case Press5_state4:
		PORTA = 0x01;
		break;
		case Press5_state5:
		PORTA = 0x02;
		break;
		case Press5_state6:
		PORTA = 0x01;
		break;
		case Press5_state7:
		PORTA = 0x04;
		break;
		case Press5_state8:
		PORTA = 0x08;
		break;
		case Press5_state9:
		PORTA = 0x02;
		break;
		case Press5_state10:
		PORTA = 0x01;
		break;
		case Press5_state11:
		PORTA = 0x02;
		break;
		case Press5_state12:
		PORTA = 0x01;
		break;
		case Press5_state13:
		PORTA = 0x04;
		break;
		case Press5_state14:
		PORTA = 0x08;
		break;
		case Press5_state15:
		PORTA = 0x01;
		break;
		case Off5:
		PORTA = 0x00;
		break;
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
			TimerSet(500);
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
			TimerSet(500);
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
		while(global_g == 4)
		{
			TimerSet(500);
			TimerOn();
			TickFct_Machine3();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 5)
		{
			TimerSet(200);
			TimerOn();
			ButtonPress3();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 6)
		{
			TimerSet(500);
			TimerOn();
			TickFct_Machine4();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 7)
		{
			TimerSet(200);
			TimerOn();
			ButtonPress4();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 8)
		{
			TimerSet(500);
			TimerOn();
			TickFct_Machine5();
			while(!TimerFlag);
			TimerFlag = 0;
		}
		while(global_g == 9)
		{
			TimerSet(200);
			TimerOn();
			ButtonPress5();
			while(!TimerFlag);
			TimerFlag = 0;
		}
	}
	return 0;
		
}