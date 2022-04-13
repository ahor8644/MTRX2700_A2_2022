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
   
  //--MANUAL INPUT--//
  bpm = 120;
  
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
   //display_tune_time(tune, bpm, total_note_elements);
   
   
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


/*void display_tune_time(char *tune, int bpm, int num_elements){


  //find each note, parse it, if valid, convert duration to time, add it to a running total, display the total at the end 
  
  int next_index = 0;
  //current and total note time in microseconds
  long int total_time_mcs = 0;
  long int note_time_mcs;
  //number of bars the song has
  long int num_bars;
  
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
  
  //all valid notes have been added, convert total to bars
  //microseconds for one bar/whole note = ((60*1000000)/bpm)*4
  //thus number of bars = time_total_mcs/(((60*1000000)/bpm)*4)
  num_bars = total_time_mcs/(((60*1000000)/bpm)*4);
  
  //CONVERTING TO DIGITS
  
  total_time = convert_to_digits_str(total_time_mcs);
  total_bars = convert_to_digits_str(num_bars);
  
  
  //PRINTING:
  print_to_serial(&sci_port, "Time of tune: ");
  print_to_serial(&sci_port, total_time);
  print_to_serial(&sci_port, "microseconds or ");
  print_to_serial(&sci_port, total_bars);
  print_to_serial(&sci_port, "bars.\n");
  
  //free char arrays holding digits
  free(total_time);
  free(total_bars);
  
}
*/



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
    of a note and finds the corresponding frequency of the note given
    a known array of base frequencies 'middle_freqs' 
  
  
  */
  int freq;
  
  //frequencies for A, B, C, D, E, F, G
  int middle_freqs[7] = {440, 494, 261, 293, 329, 349, 392};
  
  //accidental frequency addons for a#, B# (n/a), C#, D#, E# (n/a), F#, G#
  int accidental_addons[7] = {26, 0, 15, 17, 0, 21, 23};
  
  //get index corresponding to note by subtracting 'A' from it
  int pitch_index = pitch - 'A';
  
  //get the octave 4 frequency of the pitch
  freq = middle_freqs[pitch_index];
  
  //add on accidentals for given pitch
  if (accidental == '#'){
    freq += accidental_addons[pitch_index];  
  }
  
  //account for octave:
  switch (octave){
    case OCTAVE_2:  //two down from middle = /4
      freq /= 4;
      break;
    case OCTAVE_3:  //one down from middle = /2
      freq /= 2;
      break;
    case OCTAVE_5:  //one up from middle = *2
      freq *= 2;
      break;
    case OCTAVE_6:  //two up from middle = *4
      freq *= 4;
      break;
    default:
      break;
  }
  //return frequency after pitch, accidental and octave accounted for
  return freq;
}


//getting length of a note
long int get_note_length_mcs(int bpm, char duration){
 /*
   This function takes in 1) the BPM of the tune and 2) a given note's 'duration'
   /time signature to work out the time in microseconds of a requested note which
   is returned from the function. 
 */
 
 //need the length of the note and a whole note for reference
 //must be long ints as need time in microseconds for better precision (no floating points)
 long int length_mcs, whole_note_mcs;
 
 
 //find length of one whole note with given bpm
 //whole note length = (1min/bpm)*4 --> where time is in microseconds
 whole_note_mcs = ((60 * 1000000)/bpm)*4;
 
 //dividing whole note time based on the note's duration
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
 //returning note length
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
  
  //index
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
  /*
    This function takes in 1) a string and 2) the number of elements in the string
    and copies the input string into a dynamically created array WITHOUT spaces ' ',
    commas ' ' or the read_end_char (e.g. '\r').
  */
  //temporary pointer for in-function allocation
  char *temp;
  //need an expand number for reallocing
  int expand_num = 0;

  int i;
  
  //go through input, skipping spaces and commas and storing values
  for (i = 0; i < strlen(music_input); i++){
    
    //if the current char in input string is a comma, space or read end char, continue
    if ((music_input[i] == ' ') || (music_input[i] == ',') || (music_input[i] == read_end_char)){
      continue;
    }
    //if a desired character for transfer, expand destination array and store 
    else {
      //expand array using realloc
      expand_num++;
      temp = (char *)realloc(temp, expand_num*sizeof(char));
      
      //copy char into expanded spot
      temp[expand_num-1] = music_input[i];
    }
  }
  
  //add a null terminator
  temp = (char *)realloc(temp, (expand_num+1)*sizeof(char));
  temp[expand_num] = '\0';
  
  //keeping track of how many chars/note elements in the array
  *note_elements = expand_num;
  
  //returning a pointer to the dynamically allocated array
  return temp;
  
}

/*//function used in diaplying the total time
char *convert_to_digits_str(long int num_to_convert){
 
  //find number of digits in the number
  //keep multipling 10
  int digits = 1;
  long volatile int reference_num = 10;
  int i;
  char *digit_ch_arr;
  volatile int test_dig;
  
  //getting number of digits
  while (num_to_convert > reference_num){
    digits++;
    reference_num *= 10;
  }
  
  //make a char array to hold digits that is 'digits+1' elements large (1 extra for '\0')
  digit_ch_arr = (char *)malloc((digits+1)*sizeof(char));
  
  
  //set null terminator
  digit_ch_arr[digits] = '\0';
  
  
  //get each digit and store using the %10 method
  for (i = 0; i < digits; i++){
    //reverse order digits from modulo 10
    //- '0' makes them into corresponding digit
    
    test_dig = (num_to_convert%10); 
    
    digit_ch_arr[digits-1] = test_dig - '0';
    
    //divide num_to_convert by 10 and repeat:
    num_to_convert /= 10;   
  }
  
  
  //now char array filled with correct order digit chars - return pointer to this array
  return digit_ch_arr; 
}
*/


#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void music_isr(void){
  /*
    This interrupt service routine responds to a successful output compare for channel 5.
    It is initially caused by the play_note() function, after the timer interrupt is enabled
    and TC5 is set to be current time + toggle_period.
    
    Every interrupt, this function toggles the PT5/speaker output, updates a counter keeping track
    of the number of toggles/interrupts and adds toggle_period more worth of counts to the output
    compare - setting up the next toggle/interrupt.
    
    When the interrupt counter is equal to the max toggle_counter (note time is up), the isr counter
    is reset, PTT/speaker output is set to LOW and the next note flag is set to 1. 
  */
  
  
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
    
    //add current count + toggle_period * 3(counts in 1mcs @8P) to output compare
    TC5 = TCNT + (toggle_period*3);     //fast flag automatically resets C5F
  }
}
