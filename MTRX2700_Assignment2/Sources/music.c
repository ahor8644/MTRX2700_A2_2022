#include "music.h"
#include "serial.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//file-scope variables for toggling speaker on and off
//'toggle_period' = time between toggling speaker output high/low
//'toggle_counter' = total toggles for a given note time 
int toggle_period, toggle_counter, is_playing;
int next_note = 0;


char *convert_to_tune(char *music_input, int *note_elements){

  char *temp;
  int expand_num = 0;
  
  volatile char tester;

  int i;
  
  //go through input, skipping spaces and commas and storing values
  for (i = 0; i < strlen(music_input); i++){
    
    if ((music_input[i] == ' ') || (music_input[i] == ',') || (music_input[i] == read_end_char)){
      
      continue;
    } 
    else {
    
      //expand array
      expand_num++;
      temp = (char *)realloc(temp, expand_num*sizeof(char));
      
      tester = music_input[i];
      
      temp[expand_num-1] = music_input[i];
    }
  }
  
  //keeping track of how many chars/note elements in the array
  *note_elements = expand_num;
  
  return temp;
  
}

int find_next_note() {
  
}
//GENERAL MUSIC PLAYER
void music_player(void){
 
 char *music_input, *tune, *bpm_input;
 int i, total_note_elements, bpm;
 
 char pitch, accidental, duration, octave;
 
 int note_freq, note_time_ms;
 
 
 //continuous loop --> broken when 'X' inputted
 while (1){
   is_playing = 0;
   
   ///Getting BPM:
   print_to_serial(&sci_port, "Please enter a BPM or 'X' to exit.\n");
   bpm_input = get_new_command();
   bpm_input = convert_to_tune(bpm_input, &total_note_elements);
 
   //exit if X
   if (bpm_input[0] == 'X'){
    print_to_serial(&sci_port, "Exiting music function...\n");
    break;
   }  
   bpm = atoi(bpm_input);
   free(bpm_input);
   if (bpm == 0){
    print_to_serial(&sci_port, "Invalid BPM!\n");
   }
 
   //Getting song input from user:
   print_to_serial(&sci_port, "Please enter a tune in the form 'note, accidental, time, octave' or enter 'X' to exit\n");
   print_to_serial(&sci_port, "Notes: C, D, E, F, G, A, B, R(rest).\n");
   print_to_serial(&sci_port, "Accidentals: n (natural), # (sharp)\n");
   print_to_serial(&sci_port, "Duration: 1 (whole n), 2 (half n), 3 (quarter n), 4 (eighth n), 5 (sixteenth n).\n");
   print_to_serial(&sci_port, "Octaves: 2, 3, 4 (middle), 5, 6.\n");
   
   music_input = get_new_command();
   
   //Converting user input into char array with no spaces or commas 
   tune = convert_to_tune(music_input, &total_note_elements);
   

   //EXITING: if 'X', exit: free tune array and return out of function
   if (tune[0] == 'X'){
      print_to_serial(&sci_port, "Exiting music function...\n");
      free(tune);
      return;   
   }
 
   //PLAYING SONG:
   //for each note in the tune:  
   for (i = 0; i < total_note_elements; i += 4){
   
      is_playing = 1;
        
      //getting note aspects:
      pitch = tune[i];
      accidental = tune[i+1];
      duration = tune[i+2];
      octave = tune[i+3];
      
      //parse the aspects (check all of them are possible) and convert
      //if not, whole note (set of 4) is compromised, look for the next note and update 'i' (pass by ptr) 
                  //--> if no next note found, function returns 0 and for loop breaks
      
      //conversion successfull:
      if (1 == parse_note(pitch, accidental, duration, octave)){
      
        //convert the note into frequency and time (ms) 
        convert_note(bpm, pitch, accidental, duration,  &note_freq, &note_time_ms);
        
        //NOTE FREQ:
        //{C, ...,....,...,A,.....}
        //get the index --> note_index
        //{260, ...,...,...440,...}
        // freq = hz[note_index]
        
        //times or divide frequency based on octave.


        //NOTE LENGTH:
        //bps = bpm/60
        //whole note length = 1000ms/bps
        
        /*
        switch (duration){
        
          case 1:
          
            break;
            
          case 2:
          
            break;
        
            .
            .
            .
          default:
            break;
        }
        */
        
        //CONFIGURING ISR:
        //toggle period is period (peak to peak) of note divided by 2
        toggle_period = (1/note_freq)/2;
        toggle_counter = note_time_ms/toggle_period;
        
      
        //Note played by ISR:
        //wait for next note to be ready (this will be set to 1 when the isr counter is reached and note is done)
        while (!next_note){
        }
        //set next note flag back to 0
        next_note = 0;
      } 
      
      //if converting/parsing failed: find the next note
      else {
      
        //if a next note is found --> update i to that note and continue
        if (1 == find_next_note(tune, &i)){
          
          //once you have found the index of the NEXT char, do this: *i = index; -->update the for loop 'i' with that index
          //return 1 if a next note was found and 0 if you couldnt find another note
          
          continue;
        }
        //if no more notes/pitches, the tune is over! 
        else {
         break; 
        }
      }
   }
   
   //at end of tune, free tune array:
   free(tune);
 }
}
 
 
 
 
 
 
 
 
 
/* 
//bpm = 120
//bps = 120/60 = 2
//whole note length = 1000ms/bps

 
//


peak (1) -- 5ms --> low (0) -- 5ms --> peak(1)



TC5 = TCNT + toggle_period

isr:

counter++;

when counter == toggle_counter
  return

//TC5 = TCNT + toggle_period

*/

  
  
