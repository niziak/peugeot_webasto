/*
 * usart0.h
 *
 *  Created on: Sep 25, 2013
 *      Author: nizinski_w
 */

#ifndef USART0_H_
#define USART0_H_

#ifndef F_CPU
#error "Please define F_CPU"
#endif

#define USART0_BAUD             115200

//#define USART0_UBBR_VAL         ( (F_CPU) / (16UL * (USART0_BAUD)) - 1 )  // = 7,6
//#define USART0_UBBR_VAL         8   // 16Mhz clock  115200  -3,5 error

extern void USART0_vInit(void);
extern void USART0_vSendByte (unsigned char ucByte);
extern int  USART0_iSendByteToStream (unsigned char ucByte, FILE *stream);

extern BOOL USART0_bIsByteAvail(void);
extern unsigned char USART0_ucGetByte(void);

#endif /* USART0_H_ */
