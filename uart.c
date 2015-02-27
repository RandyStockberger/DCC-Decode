/*
** File:   uart.c
** Author: randy
**
** Description:
** Provide basic UART functionality:
** void uinit( uint32_t baud, uint32_t fcpu );	// Initialize UART to 'N81' and the specified baud rate
** int uputc( int ch );			// Send 8 bit character to the UART, wait if busy
** int ugetc( void );			// Get next character from the UART, wait if none available
** int uputs( char * pStr );	// Send a string of 8 bit characters to the UART
**
*/
#include <stdint.h>
#include <p24Fxxxx.h>
//
#include "decode.h"
#include "uart.h"

int InitUart1( void )
{
	// TODO: Disable analog on the Uart output pin
	TRISBbits.TRISB7 = 0;	// PORTB.7 - pin 11 is output, UART1 TX
							// PORTB.2 - pin  6 is input,  UART1 RX

	U1BRG = BRGDIVISOR;	// 103 => 9600 baud at 16MHz

	U1MODEbits.UARTEN = 1;
	U1MODE &= 0x8000;	// Reset all other bits; N,8,1 with no RTS/CTS
	U1STA = 0;		// Default all status bits
					// 832 => 1200 baud
	U1STAbits.UTXEN = 1;
	// How to enable RX?
	
	return 0;
}
int uputc( int ch )
{
#if 1
	while ( U1STAbits.UTXBF == 1 )
		;

	U1TXREG = ch;
#else
	if ( 0 == U1STAbits.UTXBF )
		U1TXREG = ch;
#endif
	return ch;
}

int ugetc( void )
{
	if ( 1 == U1STAbits.URXDA )
		return U1RXREG;
	else
		return -1;
}

// =============================================================================
// uputs -- Send a NULL terminated string to the UART
// Return:
//		-1 : error
//		n  : count of characters sent
int uputs( char * pStr )
{
	int cnt = 0;
	int ret;

	while ( *pStr ) {
		ret = uputc( *pStr );
		if ( ret == -1 )
			return -1;
	}

	return cnt;
}
