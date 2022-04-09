
#include "functions.h"
#include "derivative.h"

#include <string.h>

//BE CAREFUL, ALREADY SOMETHING WITH THE NAME 'BLANK' in derivative.h


void write_to_seg(char *num_to_display){
  
  //array of hex codes corresponding to display digits
  int display_codes[17] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, HEX_A, HEX_B, HEX_C, HEX_D, HEX_E, HEX_F, BLNK};
  int PTP_vals[4] = {0b11111110, 0b11111101, 0b11111011, 0b11110111};
  
  
  //array for holding the display code indexs of each digit to be displayed 
  volatile int digits[4];
  
  
  int i, j, k;
  int display_index;
  int blanks = 4 - strlen(num_to_display);
  
  volatile int test1;
  
  
  //turning on the 7segs:
  DDRB = 0xFF, DDRP = 0xFF, DDRJ = 0xFF;
  PTJ = 0x00;
  

  //setting the blanks
  for (k = 0; k < blanks; k++){
    
    //set the first 'ith' digits to BLANK hex codes
    digits[k] = BLNK;   
  }
  
  
  //Converting numbers to indexs
  
  test1 = num_to_display[0];
  
  for (i = blanks; i < 4; i++){
   
    
    //if current digit is a hex char: subtract 55
    if ((num_to_display[i-blanks] >= 65) && (num_to_display[i-blanks] <=70)){
      
      display_index = num_to_display[i-blanks] - 55;
      digits[i] = display_codes[display_index];
    } 
    //if current digit is a decimal digit char: subtract 48
    else if ((num_to_display[i-blanks] >= 48) && (num_to_display[i-blanks] <= 57)){
      
      display_index = (char)num_to_display[i-blanks] - 48;
      digits[i] = display_codes[display_index];
    } else{
    }
    
  }
  
  
  //DISPLAYING FOR 1 second!
  for (i = 0; i < 250; i++){
  
    for (j = 0; j < 4; j++){
     
     PTP = PTP_vals[j];
     PORTB = digits[j];
     
     delay_1ms();
     
    }
    
    
    _FEED_COP();
    
    continue; 
  }
 
 //Blank the display after 1ms
 PORTB = BLNK; 
}



void delay_1ms(void){
  /*
    This function uses the timer and the output compare channel 7
    to create a delay of 1ms (375 timer counts @64P).
  */

  TC7 = TCNT + ONEMS;
  
  //keep looping until output compare successful (1ms has passed)
  while (!TFLG1_C7F){
  }
  
  //reset output compare flag
  TFLG1_C7F = 1;
  
}
  
    


