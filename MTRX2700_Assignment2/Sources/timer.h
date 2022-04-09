#ifndef TIMER_H
#define TIMER_H


//enabling timer and TC& output compare (& interrupts)
void init_TC7(void);


//interrupt routine for when output compare is hit
__interrupt void TC7_isr(void);


#endif