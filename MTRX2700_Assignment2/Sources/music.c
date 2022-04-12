#include "music.h"
#include "serial.h"
#include "timer.h"
#include "derivative.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//file-scope variables for toggling speaker on and off
//'toggle_period' = time between toggling speaker output high/low
//'toggle_counter' = total toggles for a given note time 
int toggle_period, toggle_counter, is_playing;
int next_note = 0;
int isr_counter = 0;




//--------------------GENERAL MUSIC PLAYER--------------------
void music_player(void){
 
 char *music_input, *tune, *bpm_input, *bpm_clean;
 int i, total_note_elements, bpm;
 
 char pitch, accidental, duration, octave;
 
 int note_freq;
 long int note_length_mcs;  //note time duration in MICROSECONDS -- LONG
 

 //continuous loop --> broken when 'X' inputted
 while (1){
   
   is_playing = 0;
   
   //Getting BPM:
   print_to_serial(&sci_port, "Please enter a BPM or 'X' to exit.\n");
   bpm_input = get_new_command();
   bpm_clean = str_to_ch_arr(bpm_input, &total_note_elements);
 
   //exit if X
   if (bpm_clean[0] == 'X'){
    free(bpm_clean);
    bpm_clean = NULL;
    print_to_serial(&sci_port, "Exiting music function...\n");
    break;
   }
   //otherwise convert ch arr to bpm  
   bpm = atoi(bpm_clean);
   free(bpm_clean);
   bpm_input = NULL;
   if (bpm == 0){ //atoi returns 0 upon failure (invalid BPM input)
    print_to_serial(&sci_port, "Invalid BPM!\n");
    continue;
   }
   
   //----USER INPUTTING TUNE----
   print_to_serial(&sci_port, "\nPlease enter a tune in the form 'note, accidental, time, octave' or enter 'X' to exit\n\n");
   print_to_serial(&sci_port, "Notes:         C, D, E, F, G, A, B, R(rest).\n");
   print_to_serial(&sci_port, "Accidentals:   n (natural), # (sharp)\n");
   print_to_serial(&sci_port, "Duration:      1 (whole n), 2 (half n), 3 (quarter n), 4 (eighth n), 5 (sixteenth n).\n");
   print_to_serial(&sci_port, "Octaves:       2, 3, 4 (middle), 5, 6.\n");
   
   //getting input from serial
   music_input = get_new_command();
   
      //-----------SELECT SONGS FOR TESTING-----------
    
   //music_input = "E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,2,4,C,n,3,4,G,n,3,4,G,n,2,4,E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,D,n,3,4,D,n,3,4,E,n,3,4,D,n,3,4,C,n,2,4,R,n,3,4,D,n,3,4,E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,2,4,D,n,3,4,D,n,3,4,D,n,2,4,E,n,3,4,G,n,3,4,G,n,2,4,E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,D,n,3,4,D,n,3,4,E,n,3,4,D,n,3,4,C,n,1,4";
   //music_input = "D,n,5,4,D,n,5,4,D,n,4,5,A,n,4,4"; 
   
   //----------------------------------------------
   
   
   //converting user input into char array with no spaces or commas 
   tune = str_to_ch_arr(music_input, &total_note_elements);

   
   //PLAYING SONG: for each note in the tune:  
   for (i = 0; i < total_note_elements; i += 4){
   
      is_playing = 1;
        
      //getting note aspects:
      pitch = tune[i];
      accidental = tune[i+1];
      duration = tune[i+2];
      octave = tune[i+3];
      
      
      //conversion successfull:
      if (1 == parse_note(pitch, accidental, duration, octave)){
      
        
        //get note length in microseconds
        note_length_mcs = get_note_length_mcs(bpm, duration);
        
        //if the input is a rest: delay for 'note_length_mcs' and then continue to next char
        if (pitch == 'R'){
        
          delay_ms((note_length_mcs/1000));
          continue;
        }
        
        //if NOT a rest: get note frequency and play it
        note_freq = get_note_frequency(pitch, accidental, octave);

        
        //------playing the note------//
        play_note(note_freq, note_length_mcs);
        
        
      }  
      //if converting/parsing failed: find the next note
      else {
      
        //if a next note is found --> update i to that note and continue
        if (1 == find_next_note(tune, total_note_elements, &i)){
          
          continue;
        }
        //if no more notes/pitches, the tune is over! 
        else {
          break; 
        }
      }
   }
   
   //SONG OVER:
   free(tune);
   tune = NULL;
   print_to_serial(&sci_port, "\n\t\t\t\tTune Complete!\n"); 
 }
}



void play_note(int frequency, long int length_mcs){
  /*
    This function 
  
  */

  //toggle period is period (peak to peak) of note divided by 2
  toggle_period = (1000000/frequency)/2;
  
  //number of counts/toggles = total note time/time for one toggle
  toggle_counter = (length_mcs/toggle_period);
        
  //enabling timer interrupts:
  TIE_C5I = 1;
        
  //Set the first output compare to start the toggling process:
  TC5 = TCNT + toggle_period;
             
  
  //Note played by ISR:
  //wait for next note to be ready (this will be set to 1 when the isr counter is reached and note is done)
  while (!next_note){
  }
  
  
        
  //disabling TIMER INTERRUPTS:
  TIE_C5I = 0;
        
  //set next note flag back to 0
  next_note = 0;    
}




//parsing each note aspect:
int parse_note(char pitch, char accidental, char duration, char octave){


    //pitch must be between A and G (or R)                                   
    if(((pitch < 'A') || (pitch > 'G')) && (pitch != 'R')){
       
      print_to_serial(&sci_port, "Invalid Pitch!\n");
      return 0;
    }
    
    if((accidental != '#') && (accidental != 'n')){
         
      print_to_serial(&sci_port, "Invalid Accidental!\n");
      return 0;
      
    }
    if((duration < '1') || (duration > '5')){
      
      print_to_serial(&sci_port, "Invalid Duration!\n");   
      return 0;
    }
    if((octave < '2') || (octave > '6')){
      
      print_to_serial(&sci_port, "Invalid Octave!\n");   
      return 0;
    }      
   
    //B# isnt a note
    if((pitch == 'B') && (accidental == '#')){
      
      print_to_serial(&sci_port, "B# is not a note!\n");   
      return 0;
    }
    if ((pitch == 'E') && (accidental == '#')){
      
      print_to_serial(&sci_port, "E# is not a note!\n"); 
      return 0;
    }
   
    return 1;
 
}

//getting note frequency
int get_note_frequency(char pitch, char accidental, char octave){
 
  int freq;
  
  //frequencies for A, B, C, D, E, F, G
  int middle_freqs[7] = {440, 494, 261, 293, 329, 349, 392};
  
  //accidental frequency addons for a#, B# (n/a), C#, D#, E# (n/a), F#, G#
  int accidental_addons[7] = {26, 0, 15, 17, 0, 21, 23};
  
  //get index corresponding to note by subtracting 'A' from it
  int pitch_index = pitch - 'A';
  
  
  freq = middle_freqs[pitch_index];
  
  //adding on accidentals for given pitch
  if (accidental == '#'){
    
    freq += accidental_addons[pitch_index];  
  }
  
  //accounting for octaves:
  switch (octave){
    
    case '2':
      freq /= 4;
      break;
    case '3':
      freq /= 2;
      break;
    case '5':
      freq *= 2;
      break;
    case '6':
      freq *= 4;
      break;
    default:
      break;
  }

  return freq;
}



long int get_note_length_mcs(int bpm, char duration){
 
 long int length_mcs, whole_note_mcs;
 
 
 //find length of one whole note with given bpm
 //whole note length = (1min/bpm)*4 --> where time is in microseconds
 whole_note_mcs = ((60 * 1000000)/bpm)*4;
 
 switch (duration){
  
  case HALF_NOTE: //half note
    length_mcs = whole_note_mcs/2;
    break;
  case QUARTER_NOTE: //quarter note
    length_mcs = whole_note_mcs/4;
    break;
  case EIGHTH_NOTE: //eighth note
    length_mcs = whole_note_mcs/8;
    break;
  case SIXTEENTH_NOTE: //sixteenth note
    length_mcs = whole_note_mcs/16;
    break;
  default:  //whole note
    length_mcs = whole_note_mcs;
    break;
 }

 return length_mcs;  
}




int find_next_note(char *tune, int tune_elements, int *i){
 
  /*
    This function is called when a note element (pitch, acc., dur. octave)
    is invalid (parsing failed) and it finds the position in the string
    of the next valid note pitch.
    When found, the function updates the tune playing index 'i' (passed by ptr)
    to the position of found note/pitch and returns 1.
    If no new note is found in the rest of the string, function returns 0;
    
    Inputs:
      - 'tune' - char array holding all note elements.
      - 'tune_elements' - number of elements in the tune array.
      - 'i' - the index of the note that failed to be played.  
  */ 
 
  int j; 
  
  //start searching from the current place in the tune (i)
  //loop through each element and check if it is a note;
  for (j = *i; j < tune_elements; j++){
    
    //if current element is a notestart/pitch (A-G or R), update i and return 1
    if ( ((tune[j] >= 'A') && (tune[j] <= 'G')) || (tune[j] == 'R') ){
      
      //update general tune index 'i' to this position and return success
      *i = j;
      return 1; 
    } 
    //not a note: continue to next element
    else {
      continue;
    }    
  }
  
  //if no note has been found in the whole rest of string, function will return 0 (failure)
  return 0;
}


//converting a string to char_arr
char *str_to_ch_arr(char *music_input, int *note_elements){

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

#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void music_isr(void){
  
  //Check isr counter is equal to desired toggle counter (set according to note duration)
  if (isr_counter == toggle_counter) {      //time for current note is up:
    //reset isr counter
    isr_counter = 0;
    //flag that the speaker is ready for the next note;
    PTT = 0;
    next_note = 1;
  }
  //time ISNT up: increment counter and set new output compare time (toggle_period * 3(1ms) counts)
  else {
    isr_counter++;
    
    //toggle output by toggling OL5
    PTT ^= PTT_PTT5_MASK;
    
    TC5 = TCNT + (toggle_period*3);     //fast flag automatically resets C5F
  }
}
