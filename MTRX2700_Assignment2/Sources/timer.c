#include "timer.h"
#include "derivative.h"




//initialising timer and output compare for CH7 and CH5
void init_timer(void){
  
  TSCR1_TEN = 1;      //enable the timer 
  TSCR1_TFFCA = 1;    //set the fast flag clear (so reading TCNT to re-enables interrupt)
  
  //Setting prescaler of 8 (001)
  TSCR2_PR2 = 0;
  TSCR2_PR1 = 1;
  TSCR2_PR0 = 1;
  
  //Setting output compare for TC7
  TIOS_IOS7 = 1;      // Output compare for ch7
  TIOS_IOS5 = 1;      // Output compare for ch5
 
  //Enabling interrupt for output compare ch5
  //TIE_C5I = 1;
 
  //Enabling PT5 to be output
  DDRT_DDRT5 = 1;
   
}


void delay_ms(int ms){
  /*
    This function uses the timer and the output compare channel 7
    to create a delay of ms*1 milliseconds (nm loops of 3000 timer
    counts @8P).
  */
  int i;
  
  //Loop 1ms timer delay 'ms' times
  for (i = 0; i < ms; i++){
    
    TC7 = TCNT + ONEMS_8;
    
    //keep looping until output compare successful (1ms has passed)
    while (!TFLG1_C7F){
    }
    
    //reset output compare flag
    TFLG1_C7F = 1;
  }

}


