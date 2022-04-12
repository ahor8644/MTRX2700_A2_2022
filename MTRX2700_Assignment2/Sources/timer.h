#ifndef TIMER_H
#define TIMER_H


#define ONEMS_8 3000   //one ms is 3000 counts at 8 prescaler

/*
  Module for initialising and using the timer including setting up
  the timer and initialising registers, and a delay function using
  the output compare.
*/

//Enabling timer and TC5 & 7 output compare (and interrupts)
void init_timer(void);

//Delay function (in 1ms increments) using output compare CH7
void delay_ms(int ms);


#endif
