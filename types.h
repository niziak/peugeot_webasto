/*
 * types.h
 *
 *  Created on: Sep 5, 2013
 *      Author: nizinski_w
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef     uint8_t           BOOL;

#ifndef  FALSE
  #define FALSE   0
  #define TRUE    (!0)  // !0 is better than 1, because xxx==TRUE compares
#endif                  // explicit with the value 1 instead of bool !=0

#ifndef NULL
  #define NULL    0
#endif

#define MIN(a,b)        ((a)<(b)?(a):(b))

#endif /* TYPES_H_ */
