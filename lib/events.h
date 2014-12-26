/*
 * events.h
 *
 *  Created on: Sep 6, 2013
 *      Author: nizinski_w
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <types.h>

typedef enum
{
    EVENT_TIMER_CHECK_PATTERN = 0,  ///< timer id, first must be zero because it is used as index in @atdTimers

    EVENT_TIMER_LAST,
} EVENT_DELAYED_TIMER_ID;



#define EVENT_QUEUE_LEN 16  ///< UCHAR!

typedef enum
{
    EV_NONE = 0,

    EV_CLOCK_1S,              ///< every one second

    EV_CHECK_PATTERN      = 0x10,
    EV_WAIT_FOR_PULSES,
    EV_PULSE_TOO_LONG,
    EV_WRONG_PATTERN,
    EV_GOOD_PATTERN,

    EV_READ_TEMPERATURE,
    EV_START_WEBASTO,

    EV_UART_LINE_COMPLETE = 0x20,
    EV_UART_LINE_FULL,

} EVENT_DEF;

extern void EventPost (EVENT_DEF eEvent);
extern void EventPostFromIRQ (EVENT_DEF eEvent);
extern void EventTimerPostAfter (EVENT_DELAYED_TIMER_ID eTimerId, EVENT_DEF eEvent, unsigned long delayms);
extern void EventTimerTickEveryMS(void);
extern void EventTimerClear (EVENT_DELAYED_TIMER_ID eTimerId);

extern BOOL bIsEventWaiting(void);
extern EVENT_DEF EventGet(void);
extern void EventInit(void);

#endif /* EVENTS_H_ */
