#ifndef MUSIC_H
#define MUSIC_H



//setup output compare for channel 5:
//enable interrupt for output compare (do this in the music function)
//make a certain frequency by using a toggle period
//make an overall time by finding how many loops/counts you have to do with the given period for the time  



void music_player(char **tune, int *is_playing);

__interrupt void music_isr(void);

#endif