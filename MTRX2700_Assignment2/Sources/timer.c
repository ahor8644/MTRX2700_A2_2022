#include "timer.h"
#include "derivative.h"



//initialising timer and output compare for CH7 and CH5
void init_timer(void){

  /*
    This function enables and sets up the timer with a desired prescaler,
    output compare bits, fast flag clear and PORTT output (for functions
    like controlling the speaker). 
  */
  
  TSCR1_TEN = 1;      //enable the timer 
  TSCR1_TFFCA = 1;    //set the fast flag clear (so reading TCNT to re-enables interrupt)
  
  //Setting prescaler of 8 (001)
  TSCR2_PR2 = 0;
  TSCR2_PR1 = 1;
  TSCR2_PR0 = 1;
  
  //Setting output compare for TC7
  TIOS_IOS7 = 1;      // Output compare for ch7
  TIOS_IOS5 = 1;      // Output compare for ch5
 
  //Enabling PT5 to be output
  DDRT_DDRT5 = 1;
   
}


void delay_ms(int ms){
  /*
    This function uses the timer and the output compare channel 7
    to create a delay of 'ms'*1 milliseconds ('ms' loops of 3000 timer
    counts @8Prescaler).
  */
  int i;
  
  //Loop 1ms timer delay 'ms' times
  for (i = 0; i < ms; i++){
    
    //add 1millisecond worth of count to current time and store in out/comp7 
    TC7 = TCNT + ONEMS_8;
    
    //keep looping until output compare successful (1ms has passed)
    while (!TFLG1_C7F){
    }
    
    //reset output compare flag
    TFLG1_C7F = 1;
  }

}


