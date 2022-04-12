#include "command_parsing.h"
#include "serial.h"            

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int parse_command(SerialPort *serial_port, char **command, int num_funcs, int num_params){
  /*
    This function takes in 1) a split up command (an array of string tokens
    that have been split up around a ',') and deciphers whether or not
    the command is valid, returning 1 if so, or 0 if not.
    It also takes in the 2) serial port (for writing error messages)
    along with 3) the number of functions available for check and 4) the number
    of parameters in the command.
    
    The command/input is parsed by checking if it starts with a valid
    command marker: "`" and has a valid function choice number (within
    1 - 'num_funcs'), before storing the function choice as a variable.
    
    Depending on the funciton choice, the command is further parsed by
    calling function-specific parsing functions that check the validity
    of function specific parameters.
    
    Returns 1 upon valid command, 0 for invalid. 
  */
  
  //parsing and function flags
  int parse_successful = 1;
  int function_choice;
  
  
  
  //check that first parameter is a backtick and is of length 1 
  if ((strlen(command[0]) != 1) || (command[0][0] != '`')){
  
    print_to_serial(serial_port, "Invalid Command! First parameter must be a backtick (`).\n");
    parse_successful = 0;
  }
  
  //check that the function choice is a valid number (1-num_funcs), if not set flag to 0
  function_choice = atoi(command[1]);
  
  if ((function_choice < 1) || (function_choice > num_funcs)){
    print_to_serial(serial_port, "Invalid Command! Function number not found.\n");
    parse_successful = 0;
  }
  
  
  //now the command has been generally parsed: it needs to be specifically parsed
  switch(function_choice){
    
    case 1:
      //parse the command in accordance with flashing_function()'s requirements
      parse_successful = flashing_function_parser(serial_port, command, num_params);
      break;
     
    //no function specific parsing at this stage for music function:  
    case 2:
      break;
      
    case 3:
      //parse the command in accordance with hex_to_seg()'s requirements
      parse_successful = hex_to_seg_parser(serial_port, command, num_params); 
      break;
  }


  return parse_successful;  
}



//Function 1 (Flashing LEDs) Parser:
int flashing_function_parser(SerialPort *serial_port, char **command, int num_params){
  /*
    This function is a parser for function 1: Flashing LEDs.
    The parser checks the 3 required arguments for the Flashing LEDs function inside the
    split up command:
        
        ~ flash speed 20ms to 100ms: (1-5), number of flashes (1-100), pattern (A or B) ~   
    
    returning 0 if invalid and 1 if valid. 
  */
  
  //parsing flag and function parameter vars
  int speed, num_flashes, pattern;
  int is_valid = 1;
  
  
  //1) command must have 5 parameters (2 command, 3 function);
  if (5 != num_params){
    print_to_serial(serial_port, "Error! Flashing Function requires 3 parameters: Speed: 1 - 10, Duration (100ms): 1 - 100, Pattern: A or B.\n");  
    return 0;
  }
  
  
  //2) Speed: Must be 1-5
  speed = atoi(command[2]);   //will return 0 if conversion is unsuccessful (non numerical input)
  if ((speed < 1) || (speed > 5)){
    print_to_serial(serial_port, "Error! Invalid Flashing Speed. Allowed: 1 - 10\n");
    is_valid = 0; 
  }
  
  //3) Duration/number of flashes: Must be within 1-100
  num_flashes = atoi(command[3]);
    if ((num_flashes < 1) || (num_flashes > 100)){
    print_to_serial(serial_port, "Error! Invalid Number of Flashes requested. Allowed: 1 - 100\n");
    is_valid = 0; 
  }
  
  //4) Pattern: Either pattern A or B
  //cannot be more than one char
  if (strlen(command[4])>1){
    print_to_serial(serial_port, "Error! Flashing Pattern too long! Allowed: 'A' or 'B'\n");
    is_valid = 0;    
  }
  
  //must be A or B, if not - invalid
  pattern = command[4][0];
  if ((pattern != 'A') && (pattern != 'B')){
    print_to_serial(serial_port, "Error! Invalid Flashing Pattern. Allowed: 'A' or 'B'\n");
    is_valid = 0; 
  }

  //return validity flag
  return is_valid;  
}




//Function 3 (7-seg write) Parser:
int hex_to_seg_parser(SerialPort *serial_port, char **command, int num_params){
  /*
    This function is a parser for function 3: Displaying a HEX number on the 7-segs.
    The parser checks the 1 required arguments for the hex_to_seg() function inside the
    split up command which is:
    
    ~A maximum 4 digit HEX number with valid digits (1-9 and A-F)~  
    
    returning 0 if invalid and 1 if valid.
  */
  
  //parsing flag
  int is_valid = 1;
  
  //array for holding invalid character so it can be printed using print_to_serial() function which needs type char*
  //2 spaces (1 for invalid character and 1 for the null terminator)
  char invalid_character[2];
  
  //3rd argument in command is the 4digit string to display on the 7 seg
  char *seg_str = command[2];
  
  int i = 0;
  
  
  //check if more parameters than allowed (should be 3: 2 for command and 1 for function)
  if (num_params != 3){
    
    print_to_serial(serial_port, "Error! Wrong number of arguments for 7-seg function. Allowed: 1.\n");
    return 0;  
  }
  
  //check length of hex number input - cannot be more than 4 digits
  if (strlen(seg_str) > 4){
  
    print_to_serial(serial_port, "Error! 7-seg can only display 4 digits maximum.\n");
    is_valid = 0;
  }
  
  
  //going through each letter and checking if it is valid (0-9 or A-F)
  for (i = 0; i < strlen(seg_str); i++){
  
    _FEED_COP();
  
    //check if digit is A-F, if so continue to next
    if ((seg_str[i] >= 65) && (seg_str[i] <= 70)){
  
      continue;
    }
    //check if digit is A-F, if so continue to next
    else if ((seg_str[i] >= 48) && (seg_str[i] <= 57)){
  
      continue;
    }
    
    //if character is NOT A-F or 0-9, error
    else {
    
      //store invalid character in array which can then be written (like a formatted string)
      invalid_character[0] = seg_str[i];
      invalid_character[1] = write_end_char;
      
      print_to_serial(serial_port, "'");
      print_to_serial(serial_port, invalid_character);
      print_to_serial(serial_port, "'");
      print_to_serial(serial_port, " cannot be displayed. Displayable characters: 'A'-'F' and '0' - '9'.\n");

      is_valid = 0;
      continue;
      
    }
     
  }  
  //return validity/parsing status
  return is_valid;
  
}


char **split_up_input(char *input_string, int *number_of_parameters){
  /*
    This function takes in the 1) raw input string from the user and
    splits the char array inputted via serial into an array of strings
    delimited around commas and spaces - where each command/input parameter is
    its own element. Uses strtok()
  */
  
  int num_tokens = 1;
  int i;
  char **split_string_array;
  char *token;
  
  //Firstly, go through and change all spaces to commas (for delimiting 
  for (i = 0; i < strlen(input_string); i++){
  
    if (input_string[i] == ' '){
      
      input_string[i] = ',';
    } 
    //if CR found, change it to NULL terminator then break
    else if (read_end_char == input_string[i]){  //SHOULD BE VARIABLE - read_end_char
      
      input_string[i] = '\0';
      break;
    } 
    //commas indicate how many tokens to split up
    else if (',' == input_string[i]){
      num_tokens++;
    }
    else{
    } 
  }
  
  //store number of tokens in num_params variable in main
  //needed for parsing
  *number_of_parameters = num_tokens;
  
  //making a 2d array to fit the broken up tokens/param strings
  split_string_array = (char **)malloc(num_tokens*sizeof(char *));
  
  
  //using STRTOK() to split up string and place inside 2d array
  token = strtok(input_string, ",,");
  for (i = 0; i < num_tokens; i++){
    
    split_string_array[i] = (char *)malloc(strlen(token)*sizeof(char));
    split_string_array[i] = token;
    
    token = strtok(NULL, ",,");  
    
  }
  //return pointer to the 2d array     
  return split_string_array;
   
}


void free_str_array(char **str_arr, int num_elements){
  /*
    This function frees the dynamically allocated 2d array of strings
    1) 'str_arr' with the number of elements 2)'num_elements'
  */
  
  int i;
  
  //interate through each element/string and free it
  for (i = 0; i < num_elements; i++){
    
    //free each string element
    free(str_arr[i]);
  }
  
  //free overall array
  free(str_arr);    
}
