#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#include "header.h"


int counter = 0;
int timetrack = 0;
int Period[4] = {0, 1, 2, 4};
//char* note[2] = {"C", "1"};
int no[4] = {74, 38, 0, 0};
int period[4] = {1, 1, 2, 4};




void Init_TC5 (void) {
  // set up the timers for channel 7 to use the interrupt
  DDRT =0x20;
  TIOS = 0x20;
  TSCR1 = 0x80;
  TSCR2 = 0x7;
  timetrack = TCNT + no[1];

  TIE = 0x20;
  DDRT_DDRT5 = 1;
  
  TFLG1 = 0x20;
}


 
#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void TC5_ISR(void) { 
  
  volatile int time = TCNT;
  TC5 = time + (360-no[counter])*2;//(38-no[1])*24*10^1;     //360 is middle C , each not is 38 less (by 0.0038 = 1/24*10^7) , 38 = 1/24*10^3 (this was tuned a bit)
  
  
    
  if (time + no[counter] < timetrack + time){
    //if counter > length(note)
    counter = counter + 2;
    timetrack = time + period[counter]*24*10^7;
    
  }

  PORTB = 20;
  
  TFLG1_C5F = 1;
  PTT_PTT5 = PTT_PTT5 xor 1;
     
  

}


void main(void) {
  /* put your own code here */
  
  //int time;
  
  //int pitch[1];
  //int duration[1, 2, 4, 8];
  

   
   Init_TC5();

	EnableInterrupts;


  for(;;) {
    _FEED_COP(); /* feeds the dog */
    
  } /* loop forever */
  /* please make sure that you never leave main */
}
