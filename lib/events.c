/*
 * events.c
 *
 *  Created on: Sep 6, 2013
 *      Author: nizinski_w
 */

#include <util/atomic.h>
#include <string.h>
#include <stdio.h>

#include <config.h>
#include <log.h>
#include <tools.h>
#include "events.h"

#include <avr/pgmspace.h>

#if 0
    #define EV_DEBUG_P(a...)	DEBUG_P(a)
#else
    #define EV_DEBUG_P(a...)
#endif


typedef struct
{
    EVENT_DEF eEvent;        ///< event to post when timer expires
    unsigned long delayms;   ///< countdown timer. Event is posted when reach zero
} EVENT_DELAYED_TIMER_DEF;

static EVENT_DELAYED_TIMER_DEF  atdTimers[EVENT_TIMER_LAST];    ///< array indexed by @ref EVENT_DELAYED_TIMER_ID



#define EVENT_QUEUE_REAL_LEN (EVENT_QUEUE_LEN)+1
typedef struct
{
    EVENT_DEF           aeCurrentEvent[EVENT_QUEUE_REAL_LEN];
    UCHAR               ucReadPtr;
    UCHAR               ucWritePtr;
    BOOL                bFull;
    BOOL                bEmpty;
} FIFO_DEF;

static FIFO_DEF tdFifo;

#define ptdFifo (&tdFifo)

/**
 *  Initial state ucReadPtr = ucWritePtr = 0, bFull = FALSE;
 *  --- current implementation with fifo full flag
 *  Scenarios to handle
 *   1) ucReadPtr == ucWritePtr && bFull == FALSE   FIFO is empty
 *   2) ucReadPtr == EVENT_QUEUE_REAL_LEN           wraparound, ucReadPtr=0
 *   3) ucWritePtr == EVENT_QUEUE_REAL_LEN          wraparound, ucWritePtr=0
 *   4) bFull == TRUE                               fifo full, no write allowed
 *   5) after write: ucReadPtr == ucWritePTr        fifo full, set bFull flag
 *
 *
 *
 *  --- old implementation with walking hole to detect overflows:
 *  Scenarios to handle (previous implementation)
 *   1) ucReadPtr == ucWritePtr                 FIFO is empty
 *   2) ucReadPtr == EVENT_QUEUE_REAL_LEN       wraparound, ucReadPtr=0
 *   3) ucWritePtr == EVENT_QUEUE_REAL_LEN      wraparound, ucWritePtr=0
 *   4) ucWritePtr+1 = ucReadPtr                full, cannot write. Affter write read==write, which means empty, but it will be completely full
 *                                              so one element space is left between read and write pointers
 *   5) above with wrap around
 *
 *   During work there is always one element hole between read and write ptr to detect FIFO full
 *   Other implementation (without hole) requires additional fifo full flag
 */


static void vDumpState(void)
{
    for (UCHAR a=0; a<sizeof(ptdFifo->aeCurrentEvent); a++ )
    {
        printf_P(PSTR("%X"), (unsigned char)ptdFifo->aeCurrentEvent[a]);
    }
    printf_P(PSTR("\nr=%X w=%X\n"), ptdFifo->ucReadPtr, ptdFifo->ucWritePtr);
}

//TODO make it real mutually exclusive
void EventPostFromIRQ (EVENT_DEF eEvent)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
//        if (    (ucWritePtr + 1 == ucReadPtr) // if write position is just before read, means fifo overflow possible
//             || ( (ucWritePtr + 1 == EVENT_QUEUE_LEN) && (ucReadPtr == 0) ) // the same but for wrap around
//           )
        if ((ptdFifo->ucWritePtr == ptdFifo->ucReadPtr) && (ptdFifo->bFull))
        {
            vDumpState();
            EV_DEBUG_P(PSTR("Ffull\n"));
            breakable_delay_ms(300);
//            return;
//            for (;;);
            RESET_P(PSTR("Fifo full"));
        }

        ptdFifo->aeCurrentEvent[ptdFifo->ucWritePtr] = eEvent;
        ptdFifo->ucWritePtr++;
        if (EVENT_QUEUE_LEN == ptdFifo->ucWritePtr)
        {
            ptdFifo->ucWritePtr = 0;
        }
        // if next write position reach read pointer - fifo is full, block further writings
        if (ptdFifo->ucWritePtr == ptdFifo->ucReadPtr)
        {
            ptdFifo->bFull = TRUE;
        }
    }
}

void EventPost (EVENT_DEF eEvent)
{
    EV_DEBUG_P(PSTR("Post %02X\n"), eEvent);
    EventPostFromIRQ(eEvent);
}

/**
 * Schedule event to be posted after specified time
 *
 * To prevent dynamic creation (linked list, etc), each timer needs special ID defined in @ref EVENT_DELAYED_TIMER_ID
 *
 * @param eTimerId  timer id to use
 * @param eEvent    event to post after time
 * @param delayms   delay in ms (unsidned int)
 */
void EventTimerPostAfter (EVENT_DELAYED_TIMER_ID eTimerId, EVENT_DEF eEvent, unsigned long delayms)
{
    EV_DEBUG_P(PSTR("t %d post ev %02X after %d ms\n"), eTimerId, eEvent, delayms);
    atdTimers[eTimerId].delayms = delayms;
    atdTimers[eTimerId].eEvent = eEvent;
}

/**
 * Clears scheduled timer event
 * @param eTimerId
 */
void EventTimerClear (EVENT_DELAYED_TIMER_ID eTimerId)
{
    EV_DEBUG_P(PSTR("EventTimerClear t %d\n"), eTimerId);
    atdTimers[eTimerId].delayms = 0;
    atdTimers[eTimerId].eEvent = SYS_EVENT_NONE;
}

/**
 * Timer handling routine to be called every 1 ms from ISR
 */
void EventTimerTickEveryMS(void)
{
    UCHAR a;
    for (a=0; a<sizeof(atdTimers)/sizeof(atdTimers[0]); a++)
    {
        if (atdTimers[a].delayms > 0)
        {
            atdTimers[a].delayms--;
            if (atdTimers[a].delayms == 0)
            {
                EventPostFromIRQ(atdTimers[a].eEvent);
            }
        }
    }
}

BOOL bIsEventWaiting(void)
{
    //EV_DEBUG_P(PSTR("IsWait?\n"));
    BOOL bReturn=FALSE;
    (void)bReturn;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        //bReturn = (ptdFifo->bEmpty==TRUE ? FALSE : TRUE);
        bReturn = (ptdFifo->ucReadPtr == ptdFifo->ucWritePtr && ptdFifo->bFull==FALSE) ? FALSE : TRUE;
    }
	return bReturn;
}

EVENT_DEF EventGet(void)
{
    EVENT_DEF eRet=SYS_EVENT_NONE;

    EV_DEBUG_P(PSTR("Get\n"));
    if (bIsEventWaiting()==FALSE)
    {
        RESET_P(PSTR("GET but no ev"));
        return SYS_EVENT_NONE;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        eRet = ptdFifo->aeCurrentEvent[ptdFifo->ucReadPtr];
        ptdFifo->aeCurrentEvent[ptdFifo->ucReadPtr] = SYS_EVENT_NONE;
        ptdFifo->ucReadPtr++;
        if (EVENT_QUEUE_LEN == ptdFifo->ucReadPtr)
        {
            ptdFifo->ucReadPtr = 0;
        }
        ptdFifo->bFull = FALSE; // we got one element, so there is always space

//        // if after read, next element is equal to next write position, there is no more elements
//        if (ptdFifo->ucReadPtr == ptdFifo->ucWritePtr)
//        {
//            bFifoEmpty = TRUE;
//        }
//        else
//        {
//            bFifoEmpty = FALSE;
//        }
    }

	return eRet;
}

void EventInit(void)
{
    ptdFifo->ucReadPtr=0;
    ptdFifo->ucWritePtr=0;
    ptdFifo->bFull=0;
    ptdFifo->bEmpty=0;
    memset (ptdFifo->aeCurrentEvent, SYS_EVENT_NONE, sizeof(ptdFifo->aeCurrentEvent));
}

