/*
 * timer0.h
 *
 *  Created on: Sep 11, 2013
 *      Author: nizinski_w
 */

#ifndef TIMER0_H_
#define TIMER0_H_

#if (F_CPU==8000000UL)

    #define RESET_TIMER0_CNT    { TCNT0 = 0xFF-8+1; }              // set value - overflow every 8 ticks 128us*8=1024us

#elif (F_CPU==16000000UL)

    #define TIMER0_CNT_CRYSTAL_ADJ  (+0)                        // by hand
    //#define RESET_TIMER0_CNT    { TCNT0 = 0xFF-8-8+1; }            // set value - overflow every 16 ticks 64us*16=1024us
    #define RESET_TIMER0_CNT    { TCNT0 = 0xFF-250+1+TIMER0_CNT_CRYSTAL_ADJ; }

#else

    #error "CPU";

#endif

extern void TIMER_vInit(void);

#endif /* TIMER0_H_ */
