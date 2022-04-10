#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#include <stdio.h> 
#include <stdlib.h>


#include "serial.h"
#include "command_parsing.h"
#include "functions.h"
#include "timer.h"


//Creating a GLOBAL SERIAL PORT instance --> needed by ISR
//var holding serial port struct (known globally)
SerialPort sci_port = {&SCI1BDH, &SCI1BDL, &SCI1CR1, &SCI1CR2, &SCI1DRL, &SCI1SR1};

//USER CAN CHANGE THESE:
char write_end_char = '\0';     //variable for signifying end of a string being WRITTEN (GLOBAL)
char read_end_char = '\r';            //variable for signifying end of a string being READ (GLOBAL)



void main(void) {

//---------------------------VARIABLES--------------------------- 

  //Functions
  int func_num = 0;       //function chooser
  int number_of_functions = 3;    //number of command funcitons available to the program
  int number_of_parameters;
  
  //int test_index = 0;
  
  //Flashing Function;
  int speed, duration;
  char pattern;
  
  //Seven Deg displaying:
  char *num_to_display;
  
  
  

  //Serial Read and Write:
  char *read_string, *write_string;     //char array for reading into and writing from
  char **command;       //2d array to spli the parameters up as as strings


 
//---------------------------SETUP--------------------------- 

  //Making the watchdog timer longer (NOW 2^24):
  COPCTL |= 0b00000111;
      
 
  //Initialising the timer and output compare CH7:
  init_TC7();
  
  //initialising the SCI
  SerialInitialise(BAUD_9600, &sci_port);
  
  //setting up read interrupt;
  read_interrupt_init(&sci_port);
 

	EnableInterrupts;



//TESTING PUTTY using Polling
/*  write_string = "fuck you";


  while (1){
  
    //while register isnt ready for a new char, loop back
    while (SCI1SR1 & 0b10000000 == 0){
      _FEED_COP();
    }
    
    //is char to send is a NULL terminator - break
    if (write_string[test_index] == '\0'){
      break;
    }
    
    //put data in register to be sent out
    SCI1DRL = write_string[test_index];
    test_index++;
  }
  

  while (1){
    _FEED_COP();
  }
  
}
*/
 
//-----------------------FUNCTION LOOP-----------------------  
  while (1){
    
    _FEED_COP();        //keep feeding dog
    
    
    //wait for the new command:
    read_string = get_new_command();
    
    
    command = split_up_input(read_string, &number_of_parameters);
    
    
    
    
    if (1 == parse_command(&sci_port, command, number_of_functions, number_of_parameters)){

      //upon successful command, function number will be updated and corresponding function called
      //all other times, func_num = 0 (continue infinite loop);
      
      func_num = atoi(command[1]);
      
      switch (func_num){
        
        case 1:
          
          speed = atoi(command[2]);
          duration = atoi(command[3]);
          pattern = command[4][0];
          
          //flashing_function(speed, duration, pattern);
          break;
          
        case 2:
          //music_function();
          break;
        
        case 3:
          
          num_to_display = command[2];
          write_to_seg(num_to_display);
          break;
          
        default:
          break;
      }
      
    }
      
    free(command);
    continue;
  }
    
}
