/*
 * timer1.h
 *
 *  Created on: Dec 18, 2014
 *      Author: niziak
 */

#ifndef TIMER1_H_
#define TIMER1_H_

extern void TIMER1_vInit(void);
extern void TIMER1_vStop(void);

extern volatile uint16_t T1Ovs1, T1Ovs2;
extern volatile uint16_t Capt1, Capt2, Capt3;


#endif /* TIMER1_H_ */
