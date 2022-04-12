#ifndef COMMAND_PARSING_H
#define COMMAND_PARSING_H

#include "serial.h"


/*
  Module for handling and parsing the user inputted command read in from the SCI
  (hence dependent on serial.h)
  Includes a method of splitting the input up into an array of parameter strings
  and then parsing the command checking for both:
    - valid, general command layout
    - function specific parameter validity
  all done within the general parsing function 'parse_command()'.
*/


//End of read indicator needed by parsing function (signifies end of read in string)
extern char read_end_char;


//General parsing function - command and function parameter parsing
int parse_command(SerialPort *serial_port, char **command, int number_of_functions, int number_of_parameters);


//Function 1 (flashing function) parser: checks function specific parameters: speed, number of flashes and pattern
int flashing_function_parser(SerialPort *serial_port, char **command, int num_params);

//Function 3 (7-seg num display) parser: Parses up HEX number (up to 4 digits) to be displayed on 7-segs
int hex_to_seg_parser(SerialPort *serial_port, char **command, int num_params);


//Splits up the input into an array of parameter strings used for easier parsing
char **split_up_input(char *input_string, int *number_of_parameters);

//Freeing 2d command/split up input
void free_str_array(char **str_arr, int num_elements);


#endif
