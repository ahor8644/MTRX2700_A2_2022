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
 /*
   This function prompts the user to input a BPM amd musical notes in the form:
   
    ~pitch(A-G), accidental(n or #), duration (1-5: whole-16th), octave (2-6)~ 
    
   after which it converts the input into a cleaned (no commas or spaces) 'tune'
   /char array via str_to_ch_arr(). This tune is iterated through, and each note
   (set of 4 aspects) is parsed, before being converted, if successful, into a frequency and
   duration in microseconds. These measurements are converted into a toggle period
   (note period/2) used to toggle the speaker on and off via timer interrupts along
   with a total toggle count, which is the toggle period/note time. This note is
   played in the looping/waiting function play_note();
   
   If parsing fails, the find_next_note() function finds and updates the index to 
   be the next pitch value in the 'tune' after which this new note undergoes the same
   parsing, converting and playing function.
   
   Upon input of the tune, the function display_tune_time() checks through the whole
   tune array and adds up the total time of all valid notes and displays it.
   
   
   This function can be EXITED via inputting 'X' into either the BPM or tune input.
 */
 
 //input arrays, counters
 char *music_input, *tune, *bpm_input, *bpm_clean;
 int i, total_note_elements, bpm;
 
 //note aspects
 char pitch, accidental, duration, octave;
 
 //frequency and mcs duration needed for speaker
 int note_freq;
 long int note_length_mcs;  //note time duration in MICROSECONDS -- LONG
 

 //continuous loop for song input --> broken when 'X' inputted
 while (1){
   //update flag for currently playing
   is_playing = 0;
   
   //Getting BPM: store in char array, 'clean' array then atoi()
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
   
   //Mary had a little lamb 122BPM 
   //music_input = "E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,2,4,C,n,3,4,G,n,3,4,G,n,2,4,E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,D,n,3,4,D,n,3,4,E,n,3,4,D,n,3,4,C,n,2,4,R,n,3,4,D,n,3,4,E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,2,4,D,n,3,4,D,n,3,4,D,n,2,4,E,n,3,4,G,n,3,4,G,n,2,4,E,n,3,4,D,n,3,4,C,n,3,4,D,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,E,n,3,4,D,n,3,4,D,n,3,4,E,n,3,4,D,n,3,4,C,n,1,4";
   
   //Megalovania 120BPM
   //music_input = "Dn54Dn54Dn45An44Rn54G#54Rn54Gn54Rn54Fn54Fn54Dn54Fn54Gn54 Cn54Cn54Dn45An44Rn54G#54Rn54Gn54Rn54Fn54Fn54Dn54Fn54Gn54 Bn54Bn54Dn45An44Rn54G#54Rn54Gn54Rn54Fn54Fn54Dn54Fn54Gn54 A#54A#54Dn45An44Rn54G#54Rn54Gn54Rn54Fn54Fn54Dn54Fn54Gn54"; 
   
   //Had to do it to em:
   //music_input = "Dn24En24Cn34 En24Fn24Cn55Bn54An44 Cn24Bn24An24 Rn34Cn54Cn54Dn54En44Fn54 Cn34Bn24An34 En34Fn34Cn55Bn54An44 Dn34En34Cn34 En44Fn44Fn34Rn34Fn54Fn44Fn54 Rn34Dn44En44Fn44Fn44Gn44En44 Dn54Cn24Rn35 Rn44Dn44Dn44En44Fn44Dn34Cn44 Cn45Rn44Cn45Gn24";
   
   //----------------------------------------------
   
   
   
   
   //converting user input into char array with no spaces or commas - the TUNE 
   tune = str_to_ch_arr(music_input, &total_note_elements);

   //getting and printing the time (in ms) that it will take for the song to be played (accounting for bad notes)
   display_tune_time(tune, bpm, total_note_elements);
   
   
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
        //if no more notes/pitches (return 0), the tune is over! 
        else {
          break; 
        }
      }
   }
   
   //SONG OVER, free tune and repeat for a new song:
   free(tune);
   tune = NULL;
   print_to_serial(&sci_port, "\n\t\t\t\tTune Complete!\n\n"); 
 }
}

void display_tune_time(char *tune, int bpm, int num_elements){


  //find each note, parse it, if valid, convert duration to time, add it to a running total, display the total at the end 
  
  int next_index = 0;
  //current and total note time in microseconds
  long int total_time_mcs = 0;
  long int note_time_mcs;
  //number of bars the song has
  int num_bars;
  
  char *total_time;
  char *total_bars;
  
  volatile char pitch, accidental, duration, octave;
  
  
  //starting at index 0, find the next note/pitch
  while (1 == find_next_note(tune, num_elements, &next_index)){
 
  
    //getting note aspects:
    pitch = tune[next_index];
    accidental = tune[next_index+1];
    duration = tune[next_index+2];
    octave = tune[next_index+3];
    
    //increment index by 1 - this likely wont be a note but find_next() will update for us
    next_index++;
    
    //for next note, parse it:
    //if successful (note is valid)
    if (1 == parse_note(pitch, accidental, duration, octave)){
      
      //convert duration (valid) to time
      note_time_mcs = get_note_length_mcs(bpm, duration);   
      
      //add note's time onto the total
      total_time_mcs += note_time_mcs;
    }
    //current note is invalid: look for the next note 
    else{
      continue;
    }    
  }
  
  /*//all valid notes have been added, convert total to bars
  //microseconds for one bar/whole note = ((60*1000000)/bpm)*4
  //thus number of bars = time_total_mcs/(((60*1000000)/bpm)*4)
  num_bars = total_time_mcs/(((60*1000000)/bpm)*4);
  
  //CONVERTING TO DIGITS
  
  
  
  //converting to strings using ltoa(int, str, base)
  ltoa(total_time_mcs, total_time, 10);
  ltoa(num_bars, total_bars, 10);
  
  //PRINTING:
  print_to_serial(&sci_port, "Time of tune: ");
  print_to_serial(&sci_port, total_time);
  print_to_serial(&sci_port, "microseconds or ");
  print_to_serial(&sci_port, total_bars);
  print_to_serial(&sci_port, "bars.\n");
  */
  
}


void play_note(int frequency, long int length_mcs){
  /*
    This function takes in the 1) 'frequency' (hz) and time length 2)'length_mcs' (microsecs)
    and calculates the 'toggle_period' (period in between speaker toggles and the total
    toggle_count - amount of toggles that should occur based on overall note length.
    
    It then enables output compare 5 interrupts and begins the toggling process by 
    storing first toggle in TC5, after the program stays in a loop until the ISR is done
    and the note has been played (flagged by global variable 'next_note').
    Timer interrupts are then disabled. 
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
    /*
      This function takes in the 1) pitch, 2) accidental, 3) duration and
      4) octave of a note and parses each of them to verify if they are valid
      or not.
      
      If one note aspect fails parsing, the function returns 0 and the
      whole note is deemed invalid (after which it is skipped).
    */

    //pitch must be between A and G (or R)                                   
    if(((pitch < 'A') || (pitch > 'G')) && (pitch != 'R')){
      print_to_serial(&sci_port, "Invalid Pitch!\n");
      return 0;
    }
    //accidental can only be '#' (sharp) or 'n' (normal)
    if((accidental != '#') && (accidental != 'n')){  
      print_to_serial(&sci_port, "Invalid Accidental!\n");
      return 0;
    }
    //duration must be between 1 (whole) and 5 (sixteenth)
    if((duration < '1') || (duration > '5')){
      print_to_serial(&sci_port, "Invalid Duration!\n");   
      return 0;
    }
    //allowed octaves are between 2 and 6 (4 is the middle octave)
    if((octave < '2') || (octave > '6')){
      print_to_serial(&sci_port, "Invalid Octave!\n");   
      return 0;
    }      
    //B# isnt a note
    if((pitch == 'B') && (accidental == '#')){
      print_to_serial(&sci_port, "B# is not a note!\n");   
      return 0;
    }
    //E# isnt a note
    if ((pitch == 'E') && (accidental == '#')){
      print_to_serial(&sci_port, "E# is not a note!\n"); 
      return 0;
    }
    
    //if no failing conditions hit, return 1
    return 1;
 
}

//getting note frequency
int get_note_frequency(char pitch, char accidental, char octave){
 
  /*
    This function takes in the 1) pitch, 2) accidental, and 3) octave
    of a note 
  
  
  */
 
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
  
  //add a null terminator
  temp = (char *)realloc(temp, (expand_num+1)*sizeof(char));
  temp[expand_num] = '\0';
  
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
