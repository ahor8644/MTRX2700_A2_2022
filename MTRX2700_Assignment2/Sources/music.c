#include "music.h"




void music_player(char **tune, int number_of_notes){
  
 
 char *note;
 int note_length;
 
 //int note_freq
 //int length
 
/* 
 
 //PLAN:
 
 //FOR EACH NOTE and TIME pair:
    //note = tune[i]
    //note_length = atoi(tune[i+1]) --> if this is 0 (atoi failed) then continue to next note pair
    
    //parse the note and time and convert to frequency and length via pass by pointer
    //if (1 == parse_and_convert_note(note, note_length, &frequency, &length))
      //if parsing failed (and hence improper conversion), continue to next set of note/time -->use strcmpy() and array of values
      
    //IF SUCCESSFUL -> frequency and length (in ms) now known.
      //toggle_period = (1/frequency)/2         --> this is the time with which we want to toggle the output from high to low
      //max_counter = (note_length/toggle_period)       --> max counter at which the next_note flag is raised
    
    //PLAYING THE NOTE
    
      //enable TIE interrupts
    
        
        while (!next_note){         --> stays in a continuous loop being done by interrupts, untill counter maxes (note length finished) and while loop breaks 
        
          TC5 = TCNT + toggle_period;   --> interrupts every time
      
          next_note = 0;
        }
      
        continue;       --> move on to next note in big FOR loop
 
 //
*/ 
  
  
  
  
  
}