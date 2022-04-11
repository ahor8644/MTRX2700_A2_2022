
#include "serial.h"
#include <hidef.h>

#include <string.h>

#include "derivative.h"





//File-Scope variables TO BE USED BY ISR (that isnt needed in main):
char read_buffer[256];
char write_buffer[256];
int string_ready = 0;



//Initialising a given serial port struct
void SerialInitialise(int baud_rate, SerialPort *serial_port) {
  
  /*
    This function is called via interrupt when RDRF is raised. The
    function reads the char from the SCI data register and stores it in
    the character array 'intput_string', expanding the array with each new
    char until a '\n' is read. 
    
    Inputs:
      - serial port struct
      - char array 'input_string' for storing the read in characters.
      - static variable 'expand_num' used for expanding array and indexing for storing.
  */
  
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
    This funciton sets the RIE bit to initialise interrupts for a given
    SCI port 'serial_port' when the RDRF flag is raised to 1.
  */


  *(serial_port->control_register_2) |= SCI1CR2_RIE_MASK;  
      
}

void write_interrupt_init(SerialPort *serial_port){
 
 /*
    This funciton sets the TIE bit to initialise interrupts for a given
    SCI port 'serial_port' when the TDRE flag is raised to 1.
  */
 
  *(serial_port->control_register_2) |= SCI1CR2_SCTIE_MASK;

}

//FUNCTION FOR DISABLING





//Reading in a char
void SCI_read_char(SerialPort *serial_port, char buffer[], char end_read_indicator){
 
 /*
    This function is called via interrupt when RDRF is raised. The function
    reads a char from the SCI data register and stores it in a predefined 'buffer'
    with indexing kept by a static 'index' variable. When the read in char is
    equal to the 'end_read_indicator', the function sets the index to 0, toggles
    a variable 'string_ready' with filescope and then returns.
  
    
    Inputs:
      - serial port struct
      - char array/buffer
      - comparison char signifying end of the read/string
      - 
 */
 
 
 //GET RID OF THE BACKSLASHES BY REMOVING not appending CURRENT (backslash character) AND removing LAST CHARACTER and decrement index
 
 
 //static variable for index - holds value everytime function returns; (INIT ONCE)
 static index = 0;
 
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
  
  //reset index for next string and indicate that string has finished.
  index = 0;
  string_ready = 1;       //set the string ready flag so that get_new_command() knows to read
  return;
   
 }
 
 
 
 //BACKSPACES NOT WORKING FOR HARDWARE!!!!!:
 
 //for BACKSPACES (or DEL (127)), decrement index to be 'removed' (when next char writes over it):
 if ((read_char == '\b') || (read_char == 127)){
  
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

  //keep looping whilst command isnt ready (full command not read in yet)
  while (!string_ready){
    _FEED_COP();    //watchdog timer must be 
  }
  
  //once ready, set flag to 0 again for next string/command
  string_ready = 0;
  
  //finally, return buffer
  return read_buffer;
}




//WRITING:

void print_to_serial(SerialPort *serial_port, char *string_to_print){

  //check if TIE is set (waiting for a previous string to be full written before begining new write process'
  while (128 == (*(serial_port->control_register_2) & 0b10000000)){
  }
  
  
  //copy string literal 'string_to_print' into writebuffer
  strcpy(write_buffer, string_to_print);
  
  //enable TIE interrupts - this will begin the char by char writing
  *(serial_port->control_register_2) |= 0b10000000;
 
}


void SCI_write_char(SerialPort *serial_port, char buffer[], char end_write_indicator){

  static write_index = 0;
  
  
  //if terminating char found, do not write, and disable TIE
  if (end_write_indicator == buffer[write_index]){
    
    //disabling TIE bit by toggling using XOR
    *(serial_port->control_register_2) ^= 0b10000000;
    //set index to 0 for next string to be written
    write_index = 0;
    return;
  }
 
  //account for interrupt-register lag
  while((*(serial_port->status_register) & SCI1SR1_TDRE_MASK) == 0){
  }
  
  //if any other character, send it out and increment index for next time
  *(serial_port->data_register) = buffer[write_index];
  write_index++;
     
}



  
  
  
//ISR:
#pragma CODE_SEG __NEAR_SEG NON_BANKED


__interrupt void serial_ISR(void){
   
 

 //if TIE set, write function
 if (128 == (*(sci_port.control_register_2) & 0b10000000)){
  
  SCI_write_char(&sci_port, write_buffer, write_end_char);
 }
 //if TIE not set, interrupt mustve been for reading (checking status register instead of this cooks it)
 else {
  
  SCI_read_char(&sci_port, read_buffer, read_end_char); 
 }
  
 
    
}