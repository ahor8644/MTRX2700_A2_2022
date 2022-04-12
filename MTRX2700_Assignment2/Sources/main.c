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

  //turning off watchdog timer
  COPCTL = 0x00;    
 
  //Initialising the timer and output compare CH7:
  init_timer();
  
  //initialising the SCI
  SerialInitialise(BAUD_9600, &sci_port);
  
  //setting up read interrupt;
  read_interrupt_init(&sci_port);
 
	EnableInterrupts;


 
//-----------------------FUNCTION LOOP-----------------------  
  //Loop forever and keep asking for commands
  while (1){
    
    _FEED_COP();        //keep feeding dog
    
    //wait for the new command:
    print_to_serial(&sci_port, "\nPlease enter a command in the form  -- '`, function number, funcparam1, funcparam2, ...' --\n\n");
    print_to_serial(&sci_port, "\nFunctions:\n");
    print_to_serial(&sci_port, "Flashing Function: 1 | Parameters: a) Speed (1-5), b) Number of Flashes (1-100), c) Pattern (A/B).\n");
    print_to_serial(&sci_port, "Music Function:    2 | Parameters: NONE\n");
    print_to_serial(&sci_port, "HEX2SEG Function:  3 | Parameters: a) 4 digit HEX number. \n\n");
     
    //reading in the input from serial, updated when 'read_end_char' (\r) is pressed.
    read_string = get_new_command();
    
    //splitting up read string into an array of parameter strings (getting rid of spaces and commas)
    command = split_up_input(read_string, &number_of_parameters);
     
    
    //parsing command (both general and function specific parameters):
    //upon success (1), get the function number and call the corresponding function number
    if (1 == parse_command(&sci_port, command, number_of_functions, number_of_parameters)){
      
      //getting function number/choice
      func_num = atoi(command[1]);
      
      switch (func_num){
        
        //Flashing Function (1)
        case FLASHING_FUNC:
          //getting variables from parameters
          speed = atoi(command[2]);
          num_flashes = atoi(command[3]);
          pattern = command[4][0];
          
          print_to_serial(&sci_port, "\nEntering FLASHING function...\n\n");
          flashing_function(speed, num_flashes, pattern);
          break;
        
        //Music Function (2)  
        case MUSIC_FUNC:
          print_to_serial(&sci_port, "\nEntering MUSIC function...\n\n");
          //has no parameters - asks for a seperate input for the music
          music_player();
          break;
        
        //Hex to 7-seg Function (4)
        case HEX2SEG_FUNC:
          num_to_display = command[2];
          print_to_serial(&sci_port, "\nEntering HEX2SEG function...\n\n");
          hex_to_seg(num_to_display);
          break;
          
        default:
          break;
      }
      
    }
      
    //freeing command (2d array)
    free_str_array(command, number_of_parameters);
    continue;
  }
    
}
