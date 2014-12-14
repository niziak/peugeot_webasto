
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include <config.h>
#include <usart0.h>

static FILE USART0_stream = FDEV_SETUP_STREAM (USART0_iSendByteToStream, NULL, _FDEV_SETUP_WRITE);

void USART0_vInit(void)
{
    #undef BAUD
    #define BAUD (USART0_BAUD)
    #define BAUD_TOL 2       // baud tolerance in percent

    #include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    #if USE_2X
        UCSR0A |= _BV(U2X0);
    #else
        UCSR0A &= _BV(U2X0);
    #endif
    // Set baud rate
    //UBRR0H = (unsigned char) (USART0_UBBR_VAL >>8 );
    //UBRR0L = (unsigned char) (USART0_UBBR_VAL);

    // Enable receiver and transmitter
    UCSR0B = (
                 _BV(RXEN0)
               | _BV(TXEN0)
             );

    // set asynchronous mode, 8 bit data, NO parity, 1 bit stop
    UCSR0C = (
                 _BV(UCSZ01) | _BV(UCSZ00)      // 8 bit
             );

    stderr = stdout = &USART0_stream;
}

/**
 * Transmit single character
 * @param ucByte
 */
void USART0_vSendByte (unsigned char ucByte)
{
    // wait for data registry empty
    while ( ! (UCSR0A & (1<<UDRE0))) { };
    UDR0 = ucByte;
}

int USART0_iSendByteToStream (unsigned char ucByte, FILE *stream)
{
    // implicit CR on every NL
    if (ucByte == '\n')
    {
        USART0_vSendByte ('\r');
    }
    USART0_vSendByte (ucByte);
    return 1;
}

BOOL USART0_bIsByteAvail(void)
{

    return (UCSR0A & (1<<RXC0));
}


/**
 * Blocking!
 * @return
 */
unsigned char USART0_ucGetByte(void)
{
    // wait for data
    while ( ! (UCSR0A & (1<<RXC0))) { };
    return UDR0;
}

