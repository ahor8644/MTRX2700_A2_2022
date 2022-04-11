#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#include <stdio.h> 
#include <stdlib.h>


#include "serial.h"
#include "command_parsing.h"
#include "functions.h"
#include "timer.h"
#include "music.h"


//Creating a GLOBAL SERIAL PORT instance --> needed by ISR
//var holding serial port struct (known globally)
SerialPort sci_port = {&SCI1BDH, &SCI1BDL, &SCI1CR1, &SCI1CR2, &SCI1DRL, &SCI1SR1};

//USER CAN CHANGE THESE:
char write_end_char = '\0';     //variable for signifying end of a string being WRITTEN (GLOBAL)
char read_end_char = '\r';            //variable for signifying end of a string being READ (GLOBAL)



void main(void) {

//---------------------------VARIABLES--------------------------- 

  //Serial Read and Write:
  char *read_string;     //char array for reading into and writing from
  char **command;       //2d array to spli the parameters up as as strings

  //Functions
  int func_num = 0;       //function chooser
  int number_of_functions = 3;    //number of command funcitons available to the program
  int number_of_parameters;
  
  //Flashing Function;
  int speed, num_flashes;
  char pattern;
  
  //Seven Deg displaying:
  char *num_to_display;
  
  
//---------------------------SETUP--------------------------- 

  //Making the watchdog timer longer (NOW 2^24):
  //COPCTL |= 0b01000111;
  COPCTL = 0x00;
      
 
  //Initialising the timer and output compare CH7:
  init_TC7();
  
  //initialising the SCI
  SerialInitialise(BAUD_9600, &sci_port);
  
  //setting up read interrupt;
  read_interrupt_init(&sci_port);
 
 
 

	EnableInterrupts;


 
//-----------------------FUNCTION LOOP-----------------------  
  while (1){
    
    _FEED_COP();        //keep feeding dog
    
    
    //wait for the new command:
    print_to_serial(&sci_port, "Please enter a command in the form '`, function number, funcparam1, funcparam2, ...\n");
    read_string = get_new_command();
    command = split_up_input(read_string, &number_of_parameters);
    
    
    
    
    if (1 == parse_command(&sci_port, command, number_of_functions, number_of_parameters)){

      //upon successful command, function number will be updated and corresponding function called
      //all other times, func_num = 0 (continue infinite loop);
      
      func_num = atoi(command[1]);
      
      switch (func_num){
        
        case 1:
          
          speed = atoi(command[2]);       //speed of flash (10ms inc.)
          num_flashes = atoi(command[3]);    //duration in seconds
          pattern = command[4][0];        //A = normal, B = wave
          
          flashing_function(speed, num_flashes, pattern);
          break;
          
        case 2:  //MUSIC PLAYER
        
          music_player();
          break;
        
        case 3:
          
          num_to_display = command[2];
          hex_to_seg(num_to_display);
          break;
          
        default:
          break;
      }
      
    }
      
    //FREE INDIVIDUAL ELEMENTS IN COMMAND TOO
    free(command);
    
    continue;
  }
    
}
