/*
 * handler_app.h
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#ifndef HANDLER_APP_H_
#define HANDLER_APP_H_

extern void APP_vHandleEvent(EVENT_DEF eEvent);
extern void APP_vEnablePinChangeEvents(void);

#define PIN_CHANGE_INT_ENABLE       { PCICR  |=   _BV(PCIE0); PCMSK0 |=   _BV(PCINT0); }
#define PIN_CHANGE_INT_DISABLE      { PCICR  &= ~ _BV(PCIE0); PCMSK0 &= ~ _BV(PCINT0); }



#endif /* HANDLER_APP_H_ */
