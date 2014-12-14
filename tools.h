/*
 * tools.h
 *
 *  Created on: Sep 5, 2013
 *      Author: nizinski_w
 */

#ifndef TOOLS_H_
#define TOOLS_H_

extern void breakable_delay_break(void);
extern void breakable_delay_ms(unsigned int  __ms);
extern void vIncrementWithRange (unsigned char *pucValue, int iStep, unsigned char ucMin, unsigned char ucMax);

#endif /* TOOLS_H_ */
