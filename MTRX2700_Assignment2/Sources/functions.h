#ifndef FUNCTIONS_H
#define FUNCTIONS_H



#define ZERO 0x3F
#define ONE 0x06
#define TWO 0x5B
#define THREE 0x4F
#define FOUR 0x66
#define FIVE 0x6D
#define SIX 0x7D
#define SEVEN 0x07
#define EIGHT 0x7F
#define NINE 0x6F
#define HEX_A 0x77 
#define HEX_B 0x7C
#define HEX_C 0x39
#define HEX_D 0x5E
#define HEX_E 0x79
#define HEX_F 0x71
#define BLNK 0x00

#define ONEMS 375   //one ms is 375 counts at 64 prescaler

//Function 1: Flashing LEDs
void flashing_function(int speed, int duration, int pattern);

//Function 3: Writing hex number to 7-seg
void write_to_seg(char *num_to_display);

//Delay function (in 1ms increments)
void delay_1ms(int ms);






#endif