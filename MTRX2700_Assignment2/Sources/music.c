#include "music.h"
#include <stdlib.h>

//file-scope variables for toggling speaker on and off
//'toggle_period' = time between toggling speaker output high/low
//'toggle_counter' = total toggles for a given note time 
int toggle_period, toggle_counter;


void music_player(char **tune, int note_elements){
  
 
 //parse the total amount of note elements
 //note_elements % 3 must be 0
 
 //HOW TO DEAL WITH MISSING ELEMENTS (simply incrementing to next note will throw it off!
 //parse and convert each seperately?
 
 
 int i;
 char *pitch;
 int note_duration;
 
 int note_freq, note_time;

 //traversing through each note pair (pitch & time)
 for (i = 0; i < note_elements; i += 2){
 
    pitch = tune[i];
    note_duration = atoi(tune[i+1]);
    
    //parse and convert note to a frequency and time
    if (1 == convert_note(pitch, note_duration, note_freq, note_time)){
    
      //toggle period is period (peak to peak) of note divided by 2
      toggle_period = (1/note_freq)/2;
      //number of toggles is note's total duration/time divided by time for one toggle
      toggle_counter = note_time/toggle_period;
        
      
    }
    
    //if parsing/conversion failed, continue to next note
    continue;
 }

 
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