/*
This code was automatically generated using the Riverside-Irvine State machine Builder tool
Version 2.7 --- 5/24/2015 23:59:23 PST
*/

#include "rims.h"

unsigned char SM1_Clk;
void TimerISR() {
   SM1_Clk = 1;
}

enum SM1_States { SM1_intit, SM1_s2, SM1_s3, SM1_s4 } SM1_State;

TickFct_State_machine_1() {
   switch(SM1_State) { // Transitions
      case -1:
         SM1_State = SM1_intit;
         break;
         case SM1_intit: 
         if (A0) {
            SM1_State = SM1_s2;
         }
         break;
      case SM1_s2: 
         if (!A0) {
            SM1_State = SM1_s3;
         }
         break;
      case SM1_s3: 
         if (A0) {
            SM1_State = SM1_s4;
         }
         break;
      case SM1_s4: 
         if (!A0) {
            SM1_State = SM1_intit;
         }
         break;
      default:
         SM1_State = SM1_intit;
   } // Transitions

   switch(SM1_State) { // State actions
      case SM1_intit:
         B = 0x01;
         break;
      case SM1_s2:
         break;
      case SM1_s3:
         B = 0x02;
         break;
      case SM1_s4:
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}

int main() {

   const unsigned int periodState_machine_1 = 1000; // 1000 ms default
   TimerSet(periodState_machine_1);
   TimerOn();
   
   SM1_State = -1; // Initial state
   B = 0; // Init outputs

   while(1) {
      TickFct_State_machine_1();
      while(!SM1_Clk);
      SM1_Clk = 0;
   } // while (1)
} // Main