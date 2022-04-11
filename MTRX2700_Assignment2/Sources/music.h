#ifndef MUSIC_H
#define MUSIC_H



//setup output compare for channel 5:
//enable interrupt for output compare (do this in the music function)
//make a certain frequency by using a toggle period
//make an overall time by finding how many loops/counts you have to do with the given period for the time  



char *convert_to_tune(char *music_input, int *note_elements);

void music_player(void);




__interrupt void music_isr(void);

#endif
