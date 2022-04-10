#include "timer.h"
#include "derivative.h"




//initialising timer and output compare for CH7 and CH5
void init_TC7(void){
  
  TSCR1_TEN = 1;      //enable the timer 
  TSCR1_TFFCA = 1;    //set the fast flag clear (so reading TCNT to re-enables interrupt)
  
  //Setting prescaler of 64 (110)
  TSCR2_PR2 = 1;
  TSCR2_PR1 = 1;
  TSCR2_PR0 = 0;
  
  //Setting output compare for TC7
  TIOS_IOS7 = 1;      // Output compare for ch7
  TIOS_IOS5 = 1;      // Output compare for ch5
 
}


