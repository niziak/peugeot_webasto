/*
 * nvm.h
 *
 *  Created on: Sep 12, 2013
 *      Author: nizinski_w
 */

#ifndef NVM_H_
#define NVM_H_

#include <stdint.h>
#include <avr/eeprom.h>
#include <config.h>

extern void NVM_vLoadSettings(void);
extern void NVM_vSaveSettings(void);
extern void NVM_vRestoreFactory(void);

/**
 * HAS TO BE DEFINED BY USER AND LINKED
 */
extern void NVM_vSetDefaultValues(void);

#endif /* NVM_H_ */
