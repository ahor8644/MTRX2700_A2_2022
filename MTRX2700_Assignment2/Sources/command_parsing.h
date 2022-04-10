#ifndef COMMAND_PARSING_H
#define COMMAND_PARSING_H

#include "serial.h"


extern char read_end_char;

//splitting up the input into a array of parameter strings

char **split_up_input(char *input_string, int *number_of_parameters);

int parse_command(SerialPort *serial_port, char **command, int number_of_functions, int number_of_parameters);


//Function 1 (flashing function) parser:
int flashing_function_parser(SerialPort *serial_port, char **command, int num_params);

//Function 3 (7-seg num display) parser:
int hex_to_seg_parser(SerialPort *serial_port, char **command, int num_params);



#endif