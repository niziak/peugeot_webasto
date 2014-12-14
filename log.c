/*
 * error.c
 *
 *  Created on: Sep 6, 2013
 *      Author: nizinski_w
 */

#include <stdarg.h>
#include <stdio.h>

#include <config.h>
#include <globals.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <tools.h>

#include "log.h"

#ifdef __GNUC__
/*
 *  format (archetype, string-index, first-to-check)
 *
 * The parameter archetype determines how the format string is interpreted, and should be printf, scanf, strftime or strfmon.
 * (You can also use __printf__, __scanf__, __strftime__ or __strfmon__.)
 * The parameter string-index specifies which argument is the format string argument (starting from 1),
 * while first-to-check is the number of the first argument to check against the format string.
 * For functions where the arguments are not available to be checked (such as vprintf),
 * specify the third parameter as zero. In this case the compiler only checks the format string for consistency.
 * For strftime formats, the third parameter is required to be zero.
 * Since non-static C++ methods have an implicit this argument,
 * the arguments of such methods should be counted from two, not one, when giving values for string-index and first-to-check.
 */
extern void LOG_Log_P   (const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#endif
/**
 * Log printf like message with format string from PROGRAM MEMORY
 * NOTE: only format string (argument one) is taken from PGMEM
 * @param format
 */
extern void LOG_Log_P   (const char *format, ...)
{
    va_list args;
    va_start (args, format);

//    printf_P (PSTR("LOG: "));
    vfprintf_P (stdout, format, args);
    va_end (args);

//    wdt_reset();
//  int_delay_ms(500);
//    wdt_reset();
//  LCD_vClrScr();
}

#ifdef __GNUC__
extern void LOG_Log   (const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#endif
/**
 * Log printf like message with format string from normal RAM
 * @param format
 */
extern void LOG_Log   (const char *format, ...)
{
    va_list args;
    va_start (args, format);

//    printf_P (PSTR("LOG: "));
    vprintf (format, args);
    va_end (args);

//    wdt_reset();
//	int_delay_ms(500);
//    wdt_reset();
//	LCD_vClrScr();
}

#ifdef __GNUC__
void LOG_Reset (const char * message) __attribute__ ((format (printf, 1, 0)));
#endif
void LOG_Reset (const char * message)
{
    unsigned char ucCount;
    printf_P (PSTR("RESET:\n"));
    puts(message);

	cli(); // TODO
	for (ucCount=100 ; ucCount>0 ; ucCount--)
	{
	    wdt_reset();
	    ARDUINO_LED_ALTER
	    _delay_ms(500);
	}
	for (;;); // real reset
}

#ifdef __GNUC__
void LOG_Reset_P (const char * message) __attribute__ ((format (printf, 1, 0)));
#endif
void LOG_Reset_P (const char * message)
{
    unsigned char ucCount;
    printf_P (PSTR("RESET:\n"));
    puts_P (message);

    cli(); // TODO
    for (ucCount=100 ; ucCount>0 ; ucCount--)
    {
        wdt_reset();
        ARDUINO_LED_ALTER;
        _delay_ms(500);
    }
    for (;;); // real reset
}

/**
 * Print content of memory block in HEX block
 * @param ptr
 * @param size
 */
void LOG_vMemDump(void *ptr, UCHAR size)
{
    UCHAR ucCol=0;
    UCHAR *pucMem = (UCHAR*) ptr;
    while (size--)
    {
        LOG_Log_P(PSTR("%02X "), *pucMem++);
        ucCol++;
        if (ucCol > 7)
        {
            ucCol=0;
            LOG_NL
        }
    }
    LOG_NL
    LOG_NL
}

void LOG_vNL (void)
{
    LOG_Log_P(PSTR("\n"));
}
