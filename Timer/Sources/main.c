#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#include "timers.h"



int timerflag = 0;
int time = 0;
void main(void) {
  // demonstration of the difference with/without volatile
  //  look at the disassembled version of this code


  // set the ports for the LEDs
  DDRB= 0xFF;   /* Port B output */
  DDRJ= 0xFF;   // Port J to Output
  PTJ = 0x00;   // enable LEDs
 
  PORTB = 0x20;
 
  // call the initise timer function
  Init_TC7();
 
  // enable the interrupts
	// calls asm("CLI");
	EnableInterrupts;
	
	time = TCNT;
	
	
  
  for(;;) {
    _FEED_COP(); /* feeds the dog */
    
 
    }
    
    
  } /* loop forever */
  /* please make sure that you never leave main */
}
