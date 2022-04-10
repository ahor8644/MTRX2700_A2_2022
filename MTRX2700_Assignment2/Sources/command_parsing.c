#include "command_parsing.h"
#include "serial.h"            //for printing

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


  //"`,2,100,A"
  //char **split_up_string = ["`","2","100","A"];
  //is_valid_command = parse_command(char **split_up_string, int num_functions, char command_indicator);  
  //    inside here write functions   



char **split_up_input(char *input_string, int *number_of_parameters){
  /*
    This function splits the char array inputted via serial
    into an array of strings, where each command parameter is
    its own element.
  */
  
  int num_tokens = 1;
  int i;
  char **split_string_array;
  char *token;
  
  //Firstly, go through and change all spaces to commas
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
       
  return split_string_array;
   
}





int parse_command(SerialPort *serial_port, char **command, int num_funcs, int num_params){
  /*
    This function takes in a split up command (an array of string tokens
    that have been split up around a ',') and deciphers whether or not
    the command is valid, returning 1 if so, or 0 if not.
    
    The command/input is parsed by checking if it starts with a valid
    command marker: "`" and has a valid function choice number (within
    1 - 'num_funcs'), before storing the function choice as a variable.
    
    Depending on the funciton choice, the command is further parsed by
    calling function-specific parsing functions that check the validity
    of function specific parameters. 
  */
  
  int parse_successful = 1;
  int function_choice;
  
  
  
  //check that first parameter is a backtick and is of length 1 
  if ((strlen(command[0]) != 1) || (command[0][0] != '`')){
  
    print_to_serial(serial_port, "Invalid Command! First parameter must be a backtick (`).\n");
    parse_successful = 0;
  }
  
  //check that the function choice is a valid number (1-num_funcs)
  function_choice = atoi(command[1]);
  
  if ((function_choice < 1) || (function_choice > num_funcs)){
    print_to_serial(serial_port, "Invalid Command! Function number not found.\n");
    parse_successful = 0;
  }
  
  
  //now the command has been generally parsed: needs to be specifically parsed
  switch(function_choice){
    
    case 1:
      parse_successful = flashing_function_parser(serial_port, command, num_params);
      break;
     
    //no function specific parsing at this stage for music function:  
    case 2:
      break;
      
    case 3:
      
      parse_successful = write_to_seg_parser(serial_port, command, num_params); 
      
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
        
        flash_speed (1-10), flash_duration in 100ms increments (1-100), pattern (A or B)   
    
    returning 0 if invalid and 1 if valid. 
  */
  
  int i, speed, duration, pattern;
  int is_valid = 1;
  
  
  //1) command must have 5 parameters (2 command, 3 function);
  if (5 != num_params){
    print_to_serial(serial_port, "Error! Flashing Function requires 3 parameters: Speed: 1 - 10, Duration (100ms): 1 - 100, Pattern: A or B.\n");  
    return 0;
  }
  
  
  //2) Speed:
  speed = atoi(command[2]);   //will return 0 if conversion is unsuccessful (non numerical input)
  if ((speed < 1) || (speed > 10)){
    print_to_serial(serial_port, "Error! Invalid Flashing Speed. Allowed: 1 - 10\n");
    is_valid = 0; 
  }
  
  //3) Duration:
  duration = atoi(command[3]);
    if ((duration < 1) || (duration > 100)){
    print_to_serial(serial_port, "Error! Invalid Flashing Duration. Allowed: 1 - 100\n");
    is_valid = 0; 
  }
  
  //4) Pattern:
  if (strlen(command[4])>1){
    print_to_serial(serial_port, "Error! Flashing Pattern too long! Allowed: 'A' or 'B'\n");
    is_valid = 0;    
  }
  
  
  pattern = command[4][0];
  if ((pattern != 'A') && (pattern != 'B')){
    print_to_serial(serial_port, "Error! Invalid Flashing Pattern. Allowed: 'A' or 'B'\n");
    is_valid = 0; 
  }

  return is_valid;  
}




//Function 3 (7-seg write) Parser:
int write_to_seg_parser(SerialPort *serial_port, char **command, int num_params){
  
 
  int is_valid = 1;
  
  //array for holding invalid character so we can print it using print_to_serial() function which needs type char*
  //2 spaces - 1 for invalid character and 1 for the null terminator
  char invalid_character[2];
  
  //3rd argument in command is the 4digit string to display on the 7 seg
  char *seg_str = command[2];
  
  
  int i = 0;
  
  
  //check if more parameters than allowed (should be 3: 2 for command and 1 for function)
  if (num_params != 3){
    
    print_to_serial(serial_port, "Error! Wrong number of arguments for 7-seg function. Allowed: 1.\n");
    return 0;  
  }
  
  //check length of input
  if (strlen(seg_str) > 4){
  
    print_to_serial(serial_port, "Error! 7-seg can only display 4 digits maximum.\n");
    is_valid = 0;
  }
  
  
  //going through each letter
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
    
      //store invalid character
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
  
  return is_valid;
  
}
