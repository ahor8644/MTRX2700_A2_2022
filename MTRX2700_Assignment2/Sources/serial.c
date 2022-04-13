#include "serial.h"
#include <hidef.h>
#include <string.h>
#include "derivative.h"



//File-Scope variables TO BE USED BY ISR (that aren't needed in main):
//read and write buffers
char read_buffer[256];
char write_buffer[256];
//flag for latching a full command - get_new_command()
int string_ready = 0;



//Initialising a given serial port struct
void SerialInitialise(int baud_rate, SerialPort *serial_port) {
  /*
    This function takes in 1) a baud rate (from enum in serial.h) and
    2) a pointer to a serial port struct, and initialises the struct
    with the desired baud rate and enables transmitting and receiving
    via the port's SCXCR2 register. 
  */
  
  //desire baud rate = 24,000,000/(16*BR in registers)
  switch(baud_rate){
	case BAUD_9600:
      *(serial_port->baud_high)=0;
      *(serial_port->baud_low)=156;
	  break;
	case BAUD_19200:
      *(serial_port->baud_high)=0;
      *(serial_port->baud_low)=78;
	  break;
	case BAUD_38400:
      *(serial_port->baud_high)=0;
      *(serial_port->baud_low)=39;
	  break;
	case BAUD_57600:
      *(serial_port->baud_high)=0;
      *(serial_port->baud_low)=26;
	  break;
	case BAUD_115200:
      *(serial_port->baud_high)=0;
      *(serial_port->baud_low)=13;
	  break;
  }
  
  //enabling transmitter and receiver
  *(serial_port->control_register_2) = SCI1CR2_RE_MASK | SCI1CR2_TE_MASK;
  *(serial_port->control_register_1) = 0x00;
}



//Interrupt setups:
void read_interrupt_init(SerialPort *serial_port){
  /*
    This funciton sets the RIE bit in SCXCR2 to enable interrupts
    for the given SCI port 'serial_port' when the RDRF flag is raised to 1.
  */

  //enabling RIE (read interrupt) bit
  *(serial_port->control_register_2) |= SCI1CR2_RIE_MASK;  
      
}

void write_interrupt_init(SerialPort *serial_port){
 /*
    This funciton sets the TIE bit to initialise interrupts for a given
    SCI port 'serial_port' when the TDRE flag is raised to 1.
  */
 
 //enabling TIE (write interrupt) bit
  *(serial_port->control_register_2) |= SCI1CR2_SCTIE_MASK;

}



//Reading in a char
void SCI_read_char(SerialPort *serial_port, char buffer[], char end_read_indicator){
 
 /*
    This function is called via interrupt when RDRF is raised for the given SCI port
    1) 'serial_port'.
    The function reads a char from the SCI data register and stores it in a predefined
    2) 'buffer' with indexing kept by a static 'index' variable. When the read in char is
    equal to the 3) 'end_read_indicator', the function sets the index to 0, toggles
    a variable 'string_ready' (to signify a whole input has been read) with filescope and
    then returns. It also accounts for backspace inputs.   
 */
 
 
 //static variable for index - holds value everytime function returns; (INIT ONCE)
 static index = 0;
 
 //variable for storing read in char
 char read_char;
 volatile char reset;    //must be volatile so 'pointless' status register reading isnt skipped by compiler
 
 //to reset RDRF flag once read from, status register needs to first be read
 reset = *(serial_port->status_register);
 
 //reading in the char from data register
 read_char = *(serial_port->data_register);
 
 
 //--RDRF now 0--//
 
 
 //Storing in buffer
 buffer[index] = read_char;
 
 
 //LAST CHARACTER:
 if (read_char == end_read_indicator){
  
  //add null terminator to the position after the read end char:
  buffer[index+1] = '\0';
  //reset index for next string and indicate that string has finished.
  index = 0;
  string_ready = 1;       //set the string ready flag so that get_new_command() knows to read
  return;
   
 }
 
 
 //for BACKSPACES (or DEL (127)), decrement index to be 'removed' (when next char writes over it):
 if ((read_char == '\b') || (read_char == DEL)){
  
  //if no letters to backspace (index = 0), return out
  if (index == 0){
    return; 
  }
  index--;
  return;
 }
  
 //increment index for next char:
 index++;  
   
}

//sending string/command to main once read in:
char *get_new_command(void){

  /*
    This function transfers the read buffer contents (filled by SCI_read_char())
    to an array in main once the filescope flag 'string_ready' has been set to 1:
    signifying that the read in is complete and the buffer holds a whole string.
  */

  //keep looping whilst command/input isnt ready (full input not read in yet)
  while (!string_ready){
    _FEED_COP();    //watchdog timer must be fed whilst waiting 
  }
  
  //once ready, set flag to 0 again for next string/command
  string_ready = 0;
  
  //finally, return buffer
  return read_buffer;
}



//WRITING:
void print_to_serial(SerialPort *serial_port, char *string_to_print){
  /*
    This function takes in a 1) 'serial_port' to write from and a string
    to be written out from the port. It enables the transmit enable TIE
    bit and 'kickstarts' the process of writing out the string char by char
    via interrupts from the TDRE bit and the SCI_write_char() function.
  */

  //check if TIE is set (waiting for a previous string to be full written before begining new write process)
  while (128 == (*(serial_port->control_register_2) & SCI0CR2_SCTIE_MASK)){
  }
   
  //copy string literal 'string_to_print' into writebuffer
  strcpy(write_buffer, string_to_print);
  
  //enable TIE interrupts - this will begin the char by char writing
  *(serial_port->control_register_2) |= SCI0CR2_SCTIE_MASK;
 
}


void SCI_write_char(SerialPort *serial_port, char buffer[], char end_write_indicator){
  /*
    This function takes in 1) a 'serial_port', 2) a prefilled buffer of chars and 3) a
    array terminating character 'end_write_indicator' to write to the serial interface
    CHAR BY CHAR from the buffer using TIE interrupt enabled by print_to_serial().
    When the terminating char is found, the function disables interrupts (preventing
    recalling) and returns.
  */

  //static index to keep track of position in buffer to write from
  static write_index = 0;
  
  
  //if terminating char found, do not write, and disable TIE
  if (end_write_indicator == buffer[write_index]){
    
    //disabling TIE bit by toggling using XOR
    *(serial_port->control_register_2) ^= SCI0CR2_SCTIE_MASK;
    //set index to 0 for next string to be written
    write_index = 0;
    return;
  }
 
  //account for interrupt-register update lag by checking TDRE mask
  while((*(serial_port->status_register) & SCI1SR1_TDRE_MASK) == 0){
  }
  
  //if any other character, send it out and increment index for next time
  *(serial_port->data_register) = buffer[write_index];
  
  //incrementing index for next char (next call)
  write_index++;
     
}


//ISR:
#pragma CODE_SEG __NEAR_SEG NON_BANKED    //must be placed in the nonbanked section of memory


__interrupt void serial_ISR(void){
 /*
    This interrupt service routine responds to a SCI interrupt (from either
    the RIE or TIE bits). It checks the status of these two bits and deciphers
    whether it was a reading or writing interrupt (checking the status register
    instead makes the RDRF resetting behave strangely).
    
    If a writing based interrupt (TIE), the ISR calls the SCI_write_char() function
    which writes a char to the serial interface.
    If a reading based interrupt (RIE), the ISR calls the SCI_read_char() function
    which stores the char in the data register in the read buffer.
 */

 //if TIE (transmit enable) set, write function must've triggered
 if (128 == (*(sci_port.control_register_2) & SCI0CR2_SCTIE_MASK)){
  
  SCI_write_char(&sci_port, write_buffer, write_end_char);
 }
 //if TIE not set, interrupt must've been for reading
 else {
  
  SCI_read_char(&sci_port, read_buffer, read_end_char); 
 }
  
 
    
}
