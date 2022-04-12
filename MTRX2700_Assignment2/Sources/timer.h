#ifndef TIMER_H
#define TIMER_H


#define ONEMS_8 3000   //one ms is 3000 counts at 8 prescaler

//enabling timer and TC& output compare (& interrupts)
void init_timer(void);

//Delay function (in 1ms increments)
void delay_ms(int ms);


#endif
