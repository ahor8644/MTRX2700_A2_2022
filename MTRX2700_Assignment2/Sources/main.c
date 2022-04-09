#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#include <stdio.h> 
#include <stdlib.h>


#include "serial.h"
#include "command_parsing.h"
#include "functions.h"

 //DICK

//Creating a GLOBAL SERIAL PORT instance --> needed by ISR
//var holding serial port struct (known globally)
SerialPort sci_port = {&SCI0BDH, &SCI0BDL, &SCI0CR1, &SCI0CR2, &SCI0DRL, &SCI0SR1};

//USER CAN CHANGE THESE:
char write_end_char = '\0';     //variable for signifying end of a string being WRITTEN (GLOBAL)
char read_end_char = '\r';            //variable for signifying end of a string being READ (GLOBAL)

char *test;


//main()
void main(void) {

  int func_num = 0;       //function chooser
  int number_of_functions = 3;    //number of command funcitons available to the program
  int number_of_parameters;
  
  
  //Seven Deg displaying:
  char *num_to_display;
  
  
  
  
  
  
  char *read_string, *write_string;     //char array for reading into and writing from
  
  char **command;       //2d array to spli the parameters up as as strings
  
 //Making the watchdog timer longer (NOW 2^24):
  COPCTL |= 0b00000111;
      
   

  //initialising the serial
  SerialInitialise(BAUD_9600, &sci_port);
  
  //setting up read interrupt;
  read_interrupt_init(&sci_port);
  
	EnableInterrupts;


  
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
          //flashing_function();
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