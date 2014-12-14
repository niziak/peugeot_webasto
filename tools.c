/*
 * tools.c
 *
 *  Created on: Sep 17, 2013
 *      Author: nizinski_w
 */
#include <config.h>

static volatile BOOL bStop;

/** called from ISR to stop interruptible delay
 *
 */
void breakable_delay_break(void)
{
    bStop = TRUE;
}

/**
 * Interruptible delay (by keypress)
 * @param __ms
 */
void breakable_delay_ms(unsigned int  __ms)
{
    bStop = FALSE;
    while ((bStop==FALSE) && (__ms-->0))
    {
        _delay_ms(1);
    }
}


/**
 * Increment *pucValue (by adding iStep) and make wrap around within range ucMin and ucMax
 *
 * @param pcValue
 * @param iStep     TRUE - increment up
 * @param cMin
 * @param cMax
 */
void vIncrementWithRange (char *pcValue, int iStep, char cMin, char cMax)
{
    iStep += *pcValue;
    //CLOCK_PRINTF_P (PSTR("iStep=%d ucMax=%d ucMin=%d\n"), iStep, ucMax, ucMin);
    if (iStep > cMax)
    {
        //CLOCK_PRINTF_P(PSTR("MAX\n"));
        *pcValue = cMin;
    }
    else
    if (iStep < cMin)
    {
        //CLOCK_PRINTF_P(PSTR("MIN\n"));
        *pcValue = cMax;
    }
    else
    {
        //CLOCK_PRINTF_P(PSTR("OK\n"));
        *pcValue = iStep;
        //CLOCK_PRINTF_P(PSTR("result %d\n"), *pucValue);
    }
}
