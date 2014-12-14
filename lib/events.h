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

    SYS_1WIRE_CONVERT,
    SYS_1WIRE_READ,

#if WITH_RTC_DRIFT_MEASURE
    SYS_RTC_OFFSET_CALC_START,
    SYS_RTC_OFFSET_CALC_FINISH,
#endif

    SYS_UI_TIMEOUT,            ///< no user action - timeout from last keypress

    MENU_ACTION_UP,            ///< key up      - navigagte to previous element
    MENU_ACTION_DOWN,          ///< key down    - navigagte to next element
    MENU_ACTION_LEFT,          ///< key left    - escape / back to previous screen
    MENU_ACTION_RIGHT,         ///< key right   - enter to next screen
    MENU_ACTION_SELECT,        ///< make action on selected element

    MENU_ACTION_CONFIRMED,     ///< enter to menu was confirmed from user
//    MENU_ACTION_PAUSE,          ///< disable menu, but don't clear internal structure
//    MENU_ACTION_RESUME,         ///< enable menu, but don't reset internal structures

    DISP_BLINK,                 ///< blink flag changed

    APP_POPUP_SHOWN,            ///<

    APP_ACTIVATED,              ///< initalisation event to inform app handler about initialization (getting focus)
    APP_REACTIVATED,            ///< initalisation event to inform app handler about initialization (getting focus)
    APP_KILLED,                 ///< inform application that it was killed - to clean internal structures
    APP_LOST_CONTROL,           ///< inform application that it is not anymore foreground application (loosing focus)

#if WITH_HB_EVENT
    SYS_HEARTBEAT,
#endif

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
