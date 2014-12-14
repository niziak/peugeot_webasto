/*
 * types.h
 *
 *  Created on: Sep 5, 2013
 *      Author: nizinski_w
 */

#ifndef TYPES_H_
#define TYPES_H_

typedef		unsigned char			uint8_t;
typedef		unsigned char			u_char;
typedef		unsigned char			UCHAR;
typedef		char					CHAR;
typedef		unsigned int			UINT;
typedef		unsigned int			u_int;
typedef		int						INT;

typedef     unsigned long           ULONG;
typedef     unsigned char           BOOL;

#ifndef  FALSE
  #define FALSE   0
  #define TRUE    (!0)  // !0 is better than 1, because xxx==TRUE compares
#endif                  // explicit with the value 1 instead of bool !=0

#ifndef NULL
  #define NULL    0
#endif


#endif /* TYPES_H_ */
