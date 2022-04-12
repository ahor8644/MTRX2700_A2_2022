#ifndef MUSIC_H
#define MUSIC_H


#define HALF_NOTE '2'
#define QUARTER_NOTE '3'
#define EIGHTH_NOTE '4'
#define SIXTEENTH_NOTE '5'




void music_player(void);

int parse_note(char pitch, char accidental, char duration, char octave);


void play_note(int frequency, long int length_mcs);


int get_note_frequency(char pitch, char accidental, char octave);

long int get_note_length_mcs(int bpm, char duration);

int find_next_note(char *tune, int tune_elements, int *i);
 
char *str_to_ch_arr(char *music_input, int *note_elements);

__interrupt void music_isr(void);

#endif
