unsigned char var2, var3;
var2 = 0;
var3 = 0;


enum SM2_States {SM2_off, SM2_on1, SM2_on2, SM2_on3, SM2_on4, SM2_on5, SM2_on6} SM2_States;
void TickFct_Machine2()
{
	switch(SM2_States)
	{
		case SM2_off:
		if(~PINB & 0x10)
		{
			SM2_States = SM2_on1;
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
		SM2_States = SM2_off;
		break;
		default;
		SM2_States = SM2_off;
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

enum Press_state2{Init2, 2Press1, 2Press2, 2Press3, 2Press4, 2Press5, 2Press6, Error2, Off2} Press_state2;
void ButtonPress2()
{
	switch(Press_state2)
	{
		case Init2:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
				Press_state2 = 2Press1;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_States = Error2;
		}
		break;

		case 2Press1:
		if(~PINB & 0x04)
		{
			while(~PINB & 0x04){}
			Press_state2 = 2Press2;
		}
		if(~PINB & 0x02 || ~PINB & 0x01 || ~PINB & 0x08)
		{
			Press_States = Error;
		}
		break;

		case 2Press2:
		if(~PINB & 0x08)
		{
			while(~PINB & 0x08){}
			Press_state2 = 2Press3;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x01)
		{
			Press_state2 = Error2;
		}
		break;

		case 2Press3:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_state2 = 2Press4;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_state2 = Error2;
		}
		break;

		case 2Press4:
		if(~PINB & 0x02)
		{
			while(~PINB & 0x02){}
			Press_state2 = 2Press5;
		}
		if(~PINB & 0x01 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_state2 = Error2;
		}
		break;

		case 2Press5:
		if(~PINB & 0x01)
		{
			while(~PINB & 0x01){}
			Press_state2 = 2Press6;
		}
		if(~PINB & 0x02 || ~PINB & 0x04 || ~PINB & 0x08)
		{
			Press_state2 = Error2;
		}
		break;

		case 2Press6:
		if(~PINB & 0x10)
		{
			Press_state2 = Off2;
		}
		break;
		default:
		Press_state2 = Init2;
	}
	switch(Press_state2)
	{
		case Error2:
		PORTA = 0x10;
		break;

		case 2Press1:
		PORTA = 0x01;
		var2++;
		break;

		case 2Press2:
		PORTA 0x04;
		var2++;
		break;

		case 2Press3:
		PORTA = 0x08;
		var2++;
		break;

		case 2Press4:
		PORTA = 0x01;
		var2++;
		break;

		case 2Press5:
		PORTA = 0x02;
		var2++;
		break;

		case 2Press6:
		PORTA = 0x01;
		var2++;
		break;

		case Off2:
		PORTA = 0x00;

		default:
		break;

	}
}


enum SM3_States {SM3_off, SM3_on1, SM3_on2, SM3_on3, SM3_on4, SM3_on5, SM3_on6, SM3_on7, SM3_on8, SM3_on9} SM3_States;
void TickFct_Machine3()
{
	switch(SM3_States)
	{
		case SM3_off:
		if(~PINB & 0x10)
		{
			SM3_States = SM3_on1;
		}
		else
		{
			SM3_States = SM3_off;
		}
		break;

		case SM3_on1:
		SM3_States = SM3_on2;
		break;

		case SM3_on2:
		SM3_States = SM3_on3;
		break;

		case SM3_on3:
		SM3_States = SM3_on4;
		break;

		case SM3_on4:
		SM3_States = SM3_on5;
		break;

		case SM3_on5:
		SM3_States = SM3_on6;
		break;

		case SM3_on6:
		SM3_States = SM3_on7;
		break;

		case SM3_on7:
		SM3_States = SM3_on8;
		break;

		case SM3_on8:
		SM3_States = SM3_on9;
		break;

		case SM3_on9:
		SM3_States = SM3_off;

		default:
		SM3_States = SM3_off;

	}
}



















