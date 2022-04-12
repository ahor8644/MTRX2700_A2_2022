#ifndef FUNCTIONS_H
#define FUNCTIONS_H


//Function choices:
#define FLASHING_FUNC 1
#define MUSIC_FUNC 2
#define HEX2SEG_FUNC 3

//Flashing Function LED nums:
#define ONE_LED 0b00000001
#define TWO_LED 0b00000010
#define THREE_LED 0b00000100
#define FOUR_LED 0b00001000
#define FIVE_LED 0b00010000
#define SIX_LED 0b00100000
#define SEVEN_LED 0b01000000
#define EIGHT_LED 0b10000000



//7-seg display codes:
#define ZERO_SEG 0x3F
#define ONE_SEG 0x06
#define TWO_SEG 0x5B
#define THREE_SEG 0x4F
#define FOUR_SEG 0x66
#define FIVE_SEG 0x6D
#define SIX_SEG 0x7D
#define SEVEN_SEG 0x07
#define EIGHT_SEG 0x7F
#define NINE_SEG 0x6F
#define HEX_A_SEG 0x77 
#define HEX_B_SEG 0x7C
#define HEX_C_SEG 0x39
#define HEX_D_SEG 0x5E
#define HEX_E_SEG 0x79
#define HEX_F_SEG 0x71
#define BLNK_SEG 0x00

//Function 1: Flashing LEDs
void flashing_function(int speed, int num_flashes, int pattern);

//Function 3: Writing hex number to 7-seg
void hex_to_seg(char *num_to_display);





#endif
