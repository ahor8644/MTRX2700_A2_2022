 #include "timers.h"

// include the register/pin definitions
#include "derivative.h"      /* derivative-specific definitions */



void Init_TC7 (void) {
  // set up the timers for channel 7 to use the interrupt
  TSCR1_TEN =1;
  TSCR1_TFFCA = 1;
  TSCR2_PR2 = 1;
  TIOS_IOS7 = 1;
  TCTL1_OL7 = 1;
  
  TIE_C7I=1;
}


// look at the isr_vectors.c for where this function is 
//  added to the ISR vector table
#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void TC7_ISR(void) { 
  
  
  TC7 =TCNT + (word)62500;    // 12 Hhz and PT7
  
  PORTB ^=0xF0;        // High nibble LED on
  
  
  
}