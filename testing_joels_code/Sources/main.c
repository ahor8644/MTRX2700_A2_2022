#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#include "header.h"





void main(void) {

COPCTL = 0x00;

   
   Init_TC5();

	EnableInterrupts;


  for(;;) {
    _FEED_COP(); /* feeds the dog */
    
  } /* loop forever */
  /* please make sure that you never leave main */
}
