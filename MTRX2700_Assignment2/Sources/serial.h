#ifndef SCI_HEADER
#define SCI_HEADER

//byte declerations
#include <mc9s12dg256.h>


//Serial Port struct
typedef struct SerialPort {
  byte *baud_high;
  byte *baud_low;
  byte *control_register_1;
  byte *control_register_2;
  byte *data_register;
  byte *status_register;
} SerialPort;




//glabl variables needed from main
extern SerialPort sci_port;
extern char read_end_char, write_end_char;



enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};

 
// SerialInitialise - initialise the serial port
// Input: baud rate as defined in the enum
void SerialInitialise(int baud_rate, SerialPort *serial_port);

void read_interrupt_init(SerialPort *serial_port);
void write_interrupt_init(SerialPort *serial_port);


void SCI_read_char(SerialPort *serial_port, char buffer[], char end_read_indicator);
char *get_new_command(void);


void print_to_serial(SerialPort *serial_port, char *string_to_print);
void SCI_write_char(SerialPort *serial_port, char buffer[], char end_write_indicator);


//timer interrupt definition
__interrupt void serial_ISR(void);

 
#endif