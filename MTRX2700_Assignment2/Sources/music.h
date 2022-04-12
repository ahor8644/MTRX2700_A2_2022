#ifndef MUSIC_H
#define MUSIC_H


#define HALF_NOTE '2'
#define QUARTER_NOTE '3'
#define EIGHTH_NOTE '4'
#define SIXTEENTH_NOTE '5'

#define OCTAVE_2 '2'
#define OCTAVE_3 '3'
#define OCTAVE_5 '5'
#define OCTAVE_6 '6'

/*
  Module for defining, getting, parsing and outputting musical input from the user
  through the serial port to the speakers.
  Gets a BPM and then successive notes inputted by the user (hence using serial.h)
  defined by
  
  --note, accidental, length, octave--
   
  to form a tune. Module then plays each note by toggling on and off the speaker
  at a certain frequency (according to the pitch, accidentals and octaves) using
  output compare Pin output function of the HCS12's timer module (hence uses timer.h).
  Parses the notes as they are iterated to for playing, and are skipped if invalid.   
*/


//DISPLAYING THE TIME FOR A SONG TO BE COMPLETED!
  //find the next note, parse it, once parsed, calculate time using function and add to counter


//General/main music playing function:
void music_player(void);

//Checks the validity of every aspect for a given note - pitch, accidental, duration, octave
int parse_note(char pitch, char accidental, char duration, char octave);

//plays the single note of using timer interrupts and output pin toggling (essentially interrupt trigger and wait function)
void play_note(int frequency, long int length_mcs);

//Gets the frequency of a note corresponding to its pitch, accidental and octave
int get_note_frequency(char pitch, char accidental, char octave);

//Gets the length (in microseconds) of a note given its time signature ('duration') and the tune's BPM
long int get_note_length_mcs(int bpm, char duration);

//finds and updates player to the next note to be played upon a parsing failure for the current note
//enables the function to skip the bad note and try and find the next valid one
int find_next_note(char *tune, int tune_elements, int *i);

//removes spaces and commas from the input, giving sequential note parameter chars only 
char *str_to_ch_arr(char *music_input, int *note_elements);

//adds up the duration of all the valid notes in a tune writes total time to serial
//void display_tune_time(char *tune, int bpm, int num_elements);

//converts an integer into a character array of its digits
//char *convert_to_digits_str(long int num_to_convert);

//interrupt service routine for toggling the speaker using the timer
__interrupt void music_isr(void);

#endif

