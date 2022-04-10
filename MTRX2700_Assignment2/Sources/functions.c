#include "functions.h"
#include "derivative.h"

#include <string.h>
#include <math.h>


//Function 1: Flash LEDs
void flashing_function(int speed, int num_flashes, int pattern){

  /*
    This function flashes LEDs in either an all at once flashing pattern
    (pattern A) or a decrementing wave flashing pattern (pattern B).
    
    The function takes in three arguments:
      - 'speed' -> the speed at which the LEDs flash (increments of 20ms).
          - Speed 1 (slowest) = 200ms frequency (100ms on & off).
          - Speed 5 (fastest) = 40ms frequency (20ms on & off).
      - 'num_flashes' -> number of times the LEDs will flash (A)/cycle (B).
          - MIN 1 flash, MAX 100 flashes.
      - 'pattern' -> either normal flash (all on all off) or decrementing LED flash. 
  */
  
  int i;
  int index = 0; 
  int LED_masks[8] = {ONE_LED, TWO_LED, THREE_LED, FOUR_LED, FIVE_LED, SIX_LED, SEVEN_LED, EIGHT_LED};
  
  //speed 1 (slowest) is 100ms, speed 5 (fastest) is 20ms:
  int flash_duration = 100/speed; 

  //Configuring board ports
  DDRB = 0xFF;
  PTJ = 0x00;
   

  //Pattern 'A': Normal Flash
  if ('A' == pattern){
    
    for (i = 0; i < num_flashes; i++){

      _FEED_COP();      //feed the dog
      
      PORTB = 0xFF;
      delay_ms(flash_duration);      
      PORTB = 0x00;
      delay_ms(flash_duration);    
    }
 
  }
  //Pattern 'B': Wave Flash 
  else if ('B' == pattern){
    
    for (i = 0; i < num_flashes; i++){

      _FEED_COP();      //feed the dog
      
      PORTB = LED_masks[index];
      delay_ms(flash_duration);
      
      //reset index to 0 when it reaches last element
      if (index == 7){
        index = 0;
        continue;
      }
      index++;
    }
    //clear LEDs before exiting function
    PORTB = 0x00;
      
  } 
  else {
  }
  
}


//FUNCTION 3: Write HEX to 7-segs
void hex_to_seg(char *num_to_display){
  /*
    This function takes in a HEX number between 0x0000 - 0xFFFF and displays
    it on the 7-segment display FOR 1 SECOND.
  */  
  
  //array of hex codes corresponding to display digits
  int display_codes[17] = {ZERO_SEG, ONE_SEG, TWO_SEG, THREE_SEG, FOUR_SEG, FIVE_SEG, SIX_SEG, SEVEN_SEG, EIGHT_SEG, NINE_SEG,
                                                   HEX_A_SEG, HEX_B_SEG, HEX_C_SEG, HEX_D_SEG, HEX_E_SEG, HEX_F_SEG, BLNK_SEG};
  int PTP_vals[4] = {0b11111110, 0b11111101, 0b11111011, 0b11110111};
  
  //array for holding the display code indexs of each digit to be displayed 
  int digits[4];
  
  int i, j, k;
  int display_index;
  int blanks = 4 - strlen(num_to_display);
  
  
  //turning on the 7segs:
  DDRB = 0xFF, DDRP = 0xFF, DDRJ = 0xFF;
  PTJ = 0x00;
  

  //setting the blanks
  for (k = 0; k < blanks; k++){
    
    //set the first 'ith' digits to BLANK hex codes
    digits[k] = BLNK_SEG;   
  }
               
  
  //Converting numbers to indexs
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
     
     delay_ms(1);
     
    }
    
    
    _FEED_COP();
    
    continue; 
  }
 
 //Blank the display after 1ms
 PORTB = BLNK_SEG; 
}



void delay_ms(int ms){
  /*
    This function uses the timer and the output compare channel 7
    to create a delay of ms*1 milliseconds (nm loops of 375 timer
    counts @64P).
  */
  int i;
  
  //Loop 1ms timer delay 'ms' times
  for (i = 0; i < ms; i++){
    
    TC7 = TCNT + ONEMS;
    
    //keep looping until output compare successful (1ms has passed)
    while (!TFLG1_C7F){
    }
    
    //reset output compare flag
    TFLG1_C7F = 1;
  }

}
  
    


