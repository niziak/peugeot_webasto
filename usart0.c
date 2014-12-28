
#include <stdio.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <string.h>

#include <config.h>
#include <usart0.h>
#include <events.h>



typedef struct
{
    uint8_t au8RXLineBuffer[UART_RX_LINE_BUFFER];
    volatile uint8_t u8NextWritePos;
} RXB;

static RXB stRXB;


//static FILE USART0_stream = FDEV_SETUP_STREAM (USART0_iSendByteToStream, NULL, _FDEV_SETUP_WRITE);
static FILE USART0_stream = FDEV_SETUP_STREAM (USART0_iSendByteToStream, USART0_iReceiveByteForStream, _FDEV_SETUP_RW);


void USART0_vRXEnable(void)
{

    UCSR0B |=   ( _BV(RXEN0) | _BV(RXCIE0) );
}

void USART0_RXDisable(void)
{
    UCSR0B &= ~ ( _BV(RXEN0) | _BV(RXCIE0) );
}

/**
 *
 * @param USART_RX_vect
 */
ISR(USART_RX_vect)
{
    // Due to the buffering of the Error
    // Flags, the UCSRnA must be read before the receive buffer (UDRn)
    uint8_t u8Status = UCSR0A;
    // reading UDR clears interrupt flag
    uint8_t u8Char = UDR0;

    if ( u8Status & (_BV(FE0) |_BV(DOR0) | _BV(UPE0)) )
    {
        // ignore bad frames
        return;
    }

    switch (u8Char)
    {
        case 0x7F:  // backspace
            if (stRXB.u8NextWritePos > 0)
            {
                stRXB.u8NextWritePos--;
                USART0_iSendByteToStream(u8Char, NULL); // TODO: make TX buffered and interrupt driven
            }
            return; // ignore character (even do not echo)
            break;

        default:
            break;
    }

    #if (1) // echo enabled
    //if (stRXB.u8NextWritePos > 0)
    {
        USART0_iSendByteToStream(u8Char, NULL); // TODO: make TX buffered and interrupt driven
        //USART0_vSendByte (u8Char);
    }
    #endif

    // write if there is space in buffer
    if (stRXB.u8NextWritePos < sizeof(stRXB.au8RXLineBuffer))
    {
        stRXB.au8RXLineBuffer[stRXB.u8NextWritePos] = u8Char;
        stRXB.u8NextWritePos++;
    }
    else
    {
        USART0_RXDisable();
        EventPostFromIRQ(EV_UART_LINE_FULL);
    }

    // check end of line
    if (u8Char == '\r')
    {
        USART0_RXDisable();
        stRXB.au8RXLineBuffer[stRXB.u8NextWritePos] = 0; // null terminator
        EventPostFromIRQ(EV_UART_LINE_COMPLETE);
    }

}


void USART0_vInit(void)
{
    #undef BAUD
    #define BAUD (USART0_BAUD)
    // baud tolerance in percent
    // 115200 with CLK 16MHz gives 3% error
    #define BAUD_TOL 3       // baud tolerance in percent

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

    // Enable transmitter
    UCSR0B = (
                 0 /*_BV(RXEN0)*/
               | _BV(TXEN0)
               /*| _BV(RXCIE0)*/
             );

    // set asynchronous mode, 8 bit data, NO parity, 1 bit stop
    UCSR0C = (
                 _BV(UCSZ01) | _BV(UCSZ00)      // 8 bit
             );



    stdin = stderr = stdout = &USART0_stream;
}






/**
 * Wait for TX buffer empty
 */
void USART0_vFlush(void)
{
    loop_until_bit_is_set (UCSR0A, UDRE0); // wait for empty buffer
    //ATMEL: The TXCn Flag can be used to check that the Transmitter has completed all transfers
    //       Note that the TXCn Flag must be cleared before each transmission (before UDRn is written) if it is used for this purpose
    //loop_until_bit_is_clear (UCSR0A, TXC0); // wait for TX complete flag
    _delay_ms(1);
}

void USART0_vRXFlush(void)
{
    uint8_t u8Char;
    while ( UCSR0A & (1<<RXC0) )
        u8Char = UDR0;
    (void)u8Char;

}
/**
 * Transmit single character
 * @param ucByte
 */
void USART0_vSendByte (unsigned char ucByte)
{
    // wait for data registry empty
    loop_until_bit_is_set (UCSR0A, UDRE0);
    //ATMEL: Note that the TXCn Flag must be cleared before each transmission (before UDRn is written) if it is used for this purpose
    UCSR0A |= _BV(TXC0); // clear TXC0 by writing 1
    UDR0 = ucByte;
    //loop_until_bit_is_set(UCSR0A, TXC0);
}

int USART0_iSendByteToStream (unsigned char ucByte, FILE *stream)
{
    // implicit CR on every NL
    if (ucByte == '\n')
    {
        USART0_vSendByte ('\r');
    }
    if (ucByte == '\r')
    {
        USART0_vSendByte ('\n');
    }
    USART0_vSendByte (ucByte);
    return 1;
}

BOOL USART0_bIsByteAvail(void)
{

    return (UCSR0A & _BV(RXC0));
}


/**
 * Blocking!
 * @return
 */
unsigned char USART0_ucGetByte(void)
{
    // wait for data
    //loop_until_bit_is_set (UCSR0A, RXC0);
    while ( ! (UCSR0A & _BV(RXC0)) )
    {
        wdt_reset(); // Shity solution, but Timer0 is still counting GUI time out
    }
    return UDR0;
}

/**
 * Blocking!
 * @param stream
 * @return
 */
int USART0_iReceiveByteForStream (FILE *stream)
{
    uint8_t u8Byte = USART0_ucGetByte();
    USART0_iSendByteToStream(u8Byte, stream); // echo characters
    return u8Byte;
}

uint8_t * pu8GetLineBuf(void)
{
    return stRXB.au8RXLineBuffer;
}

void USART0_vRXWaitForLine(void)
{
    stRXB.u8NextWritePos = 0;
    memset (stRXB.au8RXLineBuffer, 0, UART_RX_LINE_BUFFER );
    USART0_vRXFlush();
    USART0_vRXEnable();
}

