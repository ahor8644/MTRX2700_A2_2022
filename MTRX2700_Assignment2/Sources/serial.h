#ifndef SCI_HEADER
#define SCI_HEADER

//byte declerations
#include <mc9s12dg256.h>

//DEL ascii value
#define DEL 127

/*
  Module for setting up and interacting with the serial port(s) on the HCS12 microcontroller.
  Defines structure for a serial port and functions to initialise the serial port structure,
  setup reading and writing interrupts, as well as functions for reading from and writing to
  the SCI and the interrupt routine that uses them.
*/




//Serial Port struct
typedef struct SerialPort {
  byte *baud_high;
  byte *baud_low;
  byte *control_register_1;
  byte *control_register_2;
  byte *data_register;
  byte *status_register;
} SerialPort;

//global variables needed from main for ISR
extern SerialPort sci_port;
extern char read_end_char, write_end_char;


//Baud Rates
enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};

 
// SerialInitialise - initialises the serial port registers and baud rate
void SerialInitialise(int baud_rate, SerialPort *serial_port);

//Read and Write interrupt setups
void read_interrupt_init(SerialPort *serial_port);
void write_interrupt_init(SerialPort *serial_port);


//Reading from SCI functions:
//getting a new input from SCI by transferring filled read-in buffer to pointer in main
char *get_new_command(void);
//reading each char from the serial port into a buffer (called by interrupt routine)
void SCI_read_char(SerialPort *serial_port, char buffer[], char end_read_indicator);


//Writing to SCI funcitons:
//initialising the writing process function
void print_to_serial(SerialPort *serial_port, char *string_to_print);
//writing each char from a buffer to the SCI (called by interrupt routine)
void SCI_write_char(SerialPort *serial_port, char buffer[], char end_write_indicator);


//timer interrupt definition
__interrupt void serial_ISR(void);

 
#endif
