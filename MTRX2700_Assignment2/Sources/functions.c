
#include "functions.h"
#include "derivative.h"

//BE CAREFUL, ALREADY SOMETHING WITH THE NAME 'BLANK' in derivative.h


void write_to_seg(char *num_to_display){
  
  //array of hex codes corresponding to display digits
  int display_codes[17] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, HEX_A, HEX_B, HEX_C, HEX_D, HEX_E, HEX_F, BLNK};
  
  //array for holding the display code indexs of each digit to be displayed 
  int digits[4];
  
  
  int i;
  int display_index;
  int blanks = 4 - strlen(num_to_display);
  
  //turning on the 7segs:
  //DDRB = 0xFF;
  //DDRP = 0xFF;
  
  
  
  //setting the blanks
  for (i = 0; i < blanks; i++){
    
    //set the first 'ith' digits to BLANK hex codes
    digits[i] = BLNK;   
  }
  
  
  //Converting numbers to indexs
  
  for (i = blanks; i < strlen(num_to_display); i++){
   
    
    //if current digit is a hex char: subtract 55
    if ((num_to_display[i] >= 65) && (num_to_display[i] <=70)){
      
      display_index = num_to_display[i] - 55;
      digits[i] = display_codes[display_index];
    } 
    //if current digit is a decimal digit char: subtract 48
    else if ((num_to_display[i] >= 48) && (num_to_display[i] <= 57)){
      
      display_index = num_to_display[i] - 48;
      digits[i] = display_codes[display_index];
    } else{
    }
    
  }
  
  
  //DISPLAYING!
  //PORTB = digits[0];
  
}
    
    


