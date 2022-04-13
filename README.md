# MTRX2700_A2_2022 README


README_2


# MTRX G5

Participants:
Angus Horton
Micheal Mei
Joel Lummis



Task 1: Serial Interrupt

Overview:
Using the serial with interrupts to write involves storing the string to be printed into a global variable. Once stored, the TIE interrupt is enabled for when the TDRE flag is enabled, which occurs when the serial is ready for the next character. This will interrupt the code to load the next character into the serial port and increment the global string containing the message. Once this is done, TDRE is set low and the interrupt is exited until the character is finished sending. If the end of a string character is detected, that character is pushed through and the index returns to 0, allowing the next message to write over the previous message. 

Serial interrupts for reading are similar to writing. For reading, the RIE bit is enabled to allow the interrupt to trigger when RDRF is raised, indicating a character has been inputted. Once the user types a character, the interrupt triggers and stores that character on a global array, incrementing the index. After being stored, the end line character is tested and if true, the code will stop looping / waiting and start processing the input. 

Instructions:
For reading, an typing can occur in the background and the string is updated/latched only when the symbol signifying the end of the read input (set by the user) is inputted to serial.
For writing, users must call print_to_serial(&sci_port, “String to print”) and put in their desired string that they would like to print.


Functions:
SerialInitialise:
This function takes in 1) a baud rate (from enum in serial.h) and 2) a pointer to a serial port struct, and initializes the struct with the desired baud rate and enables transmitting and receiving via the port's SCXCR2 register.
print_to_serial:
This function takes in a 1) 'serial_port' to write from and a string to be written out from the port. It enables the transmit enable TIE bit and 'kickstarts' the process of writing out the string char by char via interrupts from the TDRE bit and the SCI_write_char() function.
SCI_write_char: 
This function takes in 1) a 'serial_port', 2) a prefilled buffer of chars and 3) an    array terminating character 'end_write_indicator' to write to the serial interface CHAR BY CHAR from the buffer using TIE interrupt enabled by print_to_serial(). When the terminating char is found, the function disables interrupts (preventing    recalling) and returns.
SCI_read_char: 
This function is called via interrupt when RDRF is raised for the given SCI port 1) 'serial_port'.  The function reads a char from the SCI data register and stores it in a predefined 2) 'buffer' with indexing kept by a static 'index' variable. When the read-in char is equal to the 3) 'end_read_indicator', the function sets the index to 0, toggles a variable 'string_ready' (to signify a whole input has been read) with file scope and then returns. It also accounts for backspace inputs.
Get_new_command:
This function transfers the read buffer contents (filled by SCI_read_char()) to an array in main once the filescope flag 'string_ready' has been set to 1: signifying that the read in is complete and the buffer holds a whole string.
Interrupts:
serial_ISR:
This interrupt service routine responds to a SCI interrupt (from either the RIE or TIE bits). It checks the status of these two bits and deciphers whether it was a reading or writing interrupt. If a writing based interrupt (TIE), the ISR calls the SCI_write_char() function which writes a char to the serial interface. If a reading based interrupt (RIE), the ISR calls the SCI_read_char() function which stores the char in the data register in the read buffer.


Testing: 
The serial was tested heavily using the debugger, setting breakpoints after reads and analyzing the values of the read and write buffers, as well as the indexes.
Writing practice strings all close to each other (to see how it dealt with pending strings) and viewing the output.
Looking at the values of the arrays that the read buffer was ‘transferred to’.
Testing maximum input and outputs for desired behaviours - checking that the buffers were protected from overhead and write.


Discussion:
How can you buffer the data (for receive and transmit)
The incoming and sending strings are buttered through the use of variable char arrays with a maximum character count of 256. The variables are ‘read_buffer’ and ‘write_buffer’.
For reading, the read buffer is filled, and get_new_command() returns a pointer to this array on the stack.
For writing, the string literal inputted into the print_to_serial() function is copied into the write buffer, which is then sent out.
For both, a max input/output counter prevents strings that are too long from being written, and cuts off read inputs that are too long for the buffer.

How long roughly does the interrupt handler take to service the interrupt:
It will take the length of time it takes to write out one char (SCI_write_char) * the amount of chars to be written - and vice versa for reading.

Also, at a Baud rate of 9600, there are 1000 characters that can be sent out per second, hence waiting polling the TDRE bit would waste roughly 24,000,000/1000 = 24000 clock cycles per character.




Task 2
Overview:
The code functions by providing a list of acceptable inputs to the user via writing to serial port. The code then loops passively, using interrupts to read and store characters inputted by the user. Once the user inputs a new line character, the input string is broken up into an array of characters which determine what function and the parameters have been selected. The input validity is tested, and if invalid an error message corresponding to what was unacceptable is sent to the serial. If acceptable, the argument is used to determine which function is chosen. The available functions are: music, flashing LEDs, writing to 7 seg which will do certain things depending on the input.

Instructions:
A command is inputted as:
 ‘`(backtick), function number (1-3), functionparam1, functionparam2….’

The print functions tell the user the allowed inputs for each of the 3 functions
And the command is dealt with when the user inputs the end read char.

Functions:
Parse_command:
This function takes in 1) a split up command (an array of string tokens that have been split up around a ',') and deciphers whether or not the command is valid, returning 1 if so, or 0 if not. It also takes in the 2) serial port (for writing error messages) along with 3) the number of functions available for check and 4) the number of parameters in the command. The command/input is parsed by checking if it starts with a valid command marker: "`" and has a valid function choice number (within 1 - 'num_funcs'), before storing the function choice as a variable. Depending on the function choice, the command is further parsed by calling function-specific parsing functions that check the validity of function specific parameters.
Flashing_function_parser:
This function is a parser for function 1: Flashing LEDs. The parser checks the 3 required arguments for the Flashing LEDs function inside the split up command:
hex_to_seg_parser:
This function is a parser for function 3: Displaying a HEX number on the 7-segs.  The parser checks the 1 required arguments for the hex_to_seg() function inside the split up command which is:
split_up_input:
This function takes in the 1) raw input string from the user and splits the char array inputted via serial into an array of strings delimited around commas and spaces - where each command/input parameter is its own element. Uses strtok()
free_str_array
This function frees the dynamically allocated 2d array of strings 1) 'str_arr' with the number of elements 2)'num_elements'.
Init_timer:
This function enables and sets up the timer with a desired prescaler, output compare bits, fast flag clear and PORTT output (for functions like controlling the speaker). 
delay_ms:
This function uses the timer and the output compare channel 7 to create a delay of 'ms'*1 milliseconds ('ms' loops of 3000 timer counts at 8Prescaler).
Flashing_function:
This function flashes LEDs in either an all at once flashing pattern (pattern A) or a decrementing wave flashing pattern (pattern B).
hex_to_seg
This function takes in a HEX number between 0x0000 - 0xFFFF and displays it on the 7-segment display FOR 1 SECOND.


Testing: 
Observing correct response to serial input for each function.
With a given string, analyzing how the the string was broken up into an array of strings using the split_up_string();
Following the logic for a given incorrect string and observing the desired error messages and return values.
This includes extreme inputs, too many and too few inputs, borderline inputs, function specific input errors etc.

Discussion:
How easy would it be to extend this to different types of messages?
It would be very easy. All you have to do is change that character which signifies a command in the parsing function.
To add functions, all you would have to do is add the function to main, create a new function as well as a function specific parser to be put inside the parse_command() function.

What ways could the parser fail?
The parser only fails if the command signifier is wrong, function number is invalid or the function specific parameters are invalid (for the given function number/choice).
It will also fail if any of these are out of the order:
‘`, func_num, param1, param2….’


Task 3: Music
Overview:
The code will initially prompt the user for a tune, asking for: note, accidental, duration and octave. Once these parameters are inputted and tested on their validity (including impossible notes like B#) the tune is broken up into duration and frequency. Invalid inputs are skipped until the next valid input can be found. The timer interrupt is used to convert the frequency into tone by oscillating on and off at the inputted frequency. The duration of each note is also inputted into the timer interrupt, and once the timer has reached a set duration, the input array is incremented for the next note until the tune is over.

Instructions:
Once the music function is entered:
The user can input a song by inputting successive notes into the terminal, each in the form:
‘Pitch (A-G or R), Accidental (‘n’ or ‘#’), Time signature (1-5: whole-16th), octave (2-6: mid4)’

Once the song ends they will be prompted for another tune.


Functions:
music_player:
This function prompts the user to input a BPM amd musical notes after which it converts the input into a cleaned (no commas or spaces) 'tune' /char array via str_to_ch_arr(). This tune is iterated through, and each note (set of 4 aspects) is parsed, before being converted, if successful, into a frequency and duration in microseconds. These measurements are converted into a toggle period (note period/2) used to toggle the speaker on and off via timer interrupts along with a total toggle count, which is the toggle period/note time. This note is played in the looping/waiting function play_note(); If parsing fails, the find_next_note() function finds and updates the index to be the next pitch value in the 'tune' after which this new note undergoes the same parsing, converting and playing function. Upon input of the tune, the function display_tune_time() checks through the whole tune array and adds up the total time of all valid notes and displays it.
display_tune_time:
find each note, parse it, if valid, convert duration to time, add it to a running total, display the total at the end
play_note
This function takes in the 1) 'frequency' (hz) and time length 2)'length_mcs'  microsecs) and calculates the 'toggle_period' (period in between speaker toggles and the total toggle_count - amount of toggles that should occur based on overall note length. It then enables output compare 5 interrupts and begins the toggling process by storing first toggle in TC5, after the program stays in a loop until the ISR is done and the note has been played (flagged by global variable 'next_note'). Timer interrupts are then disabled. 
parse_note:
This function takes in the 1) pitch, 2) accidental, 3) duration and 4) octave of a note and parses each of them to verify if they are valid or not. If one note aspect fails parsing, the function returns 0 and the whole note is deemed invalid (after which it is skipped).
get_note_frequency:
This function takes in the 1) pitch, 2) accidental, and 3) octave of a note and finds the corresponding frequency of the note given a known array of base frequencies 'middle_freqs' 
get_note_length_mcs
This function takes in 1) the BPM of the tune and 2) a given note's 'duration' /time signature to work out the time in microseconds of a requested note which is returned from the function. 
find_next_note:
This function is called when a note element (pitch, acc., dur. octave) is invalid (parsing failed) and it finds the position in the string of the next valid note pitch. When found, the function updates the tune playing index 'i' (passed by ptr) to the position of found note/pitch and returns 1. If no new note is found in the rest of the string, function returns 0;
str_to_ch_arr:
This function takes in 1) a string and 2) the number of elements in the string and copies the input string into a dynamically created array WITHOUT spaces ' ', commas ' ' or the read_end_char (e.g. '\r').
Interrupt:
music_isr:
This interrupt service routine responds to a successful output compare for channel 5. It is initially caused by the play_note() function, after the timer interrupt is enabled and TC5 is set to be current time + toggle_period. Every interrupt, this function toggles the PT5/speaker output, updates a counter keeping track
of the number of toggles/interrupts and adds toggle_period more worth of counts to the output compare - setting up the next toggle/interrupt. When the interrupt counter is equal to the max toggle_counter (note time is up), the isr counter
is reset, PTT/speaker output is set to LOW and the next note flag is set to 1. 

Testing: 
Testing variety of wrong inputs in various orders and forms
Using correct inputs and observing the corresponding sounds from the microcontroller.
Playing incorrect strings, and testing that the incorrect parts were skipped.
Breaking through the program and analyzing correct variable values in the debugger.
Changing note time signature and timing the note length (seeing if they roughly add up).

Discussion:
What happens if a new tune is requested before the previous one finishes?
The method that we have implemented does not account for new tune requests whilst the tune is being played. This is because it latches the input from the user outside of the tune playing portion. The get_new_command() function waits for the user to input a end_read_char (such as a ‘\r’), after which a global flag ‘string_ready’ allows the function to exit the waiting loop and pass the finished read buffer. To make it non-blocking, we would instead have to change the function from returning the read_buffer to updating a given pointer via pass by pointer for transferring the read buffer. If this was done, we would have an option to either wait for the new input or just return out of the function and maintain the contents of the old one. WIth this, a global variable signifying whether a tune is playing (such as ‘is_playing’) could be used to decided to either wait (tune not playing), or continue and only latch when string_ready is raised to 1 (instead of waiting for it). This would enable us to call this function after playing every note. When a new tune would be requested, the note index would be set back to 0 to begin playing from the start of the new tune. 

What happens if there are unexpected characters or parsing errors from the tune string.
For each note, the 4 aspects are parsed and if any of them fail, the whole note is deemed invalid and the function find_next_note() looks for the next pitch in the tune array and does the same process (parsing) until a valid note is found, by which is updates the loop index to be this position and plays the note. Hence, every incorrect note is skipped. If no new notes are left, the tune playing portion exits and users are prompted for a new tune input.
