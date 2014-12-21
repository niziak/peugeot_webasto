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
    EVENT_TIMER_1WIRE = 0,  ///< timer id, first must be zero because it is used as index in @atdTimers
    EVENT_TIMER_POPUP,
    #if WITH_RTC_DRIFT_MEASURE
    EVENT_TIMER_RTC_OFFSET, ///< RTC drift calculation
    #endif

    EVENT_TIMER_LAST,
} EVENT_DELAYED_TIMER_ID;



#define EVENT_QUEUE_LEN 16  ///< UCHAR!

typedef enum
{
    SYS_EVENT_NONE = 0,

    SYS_CLOCK_1S,              ///< every one second

    SYS_PB0_HI,					///< PB0 was changed to HI state
    SYS_T1INT,

    SYS_CHECK_PULSES,
    SYS_GO_TO_SLEEP,
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
