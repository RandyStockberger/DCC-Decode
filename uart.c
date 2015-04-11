/*
** File:   uart.c
** Author: randy
**
** Description:
** Provide UART functionality that communicates directly with the UART
**
** int InitUart1( uint32_t baud, uint32_t fcpu );	// Initialize UART to 'N81'
** int uputc( int ch );			// Send 8 bit character to the UART, wait if busy
** int ugetc( void );			// Get next character from the UART, wait if none available
**
*/

#include <stdint.h>
#include <p24Fxxxx.h>
//
#include "decode.h"
#include "uart.h"

// ============================================================================
//
// InitUart1 -- Do hardware initialization on the UART
//
int InitUart1( void )
{
	TRISBbits.TRISB7 = 0;	// PORTB.7 - pin 11 is output, UART1 TX
	TRISBbits.TRISB2 = 1;	// PORTB.2 - pin  6 is input,  UART1 RX

	U1BRG = BRGDIVISOR;		// 103 => 9600 baud at 16MHz
	U1MODEbits.UARTEN = 1;
	U1MODE &= 0x8000;		// Reset all other bits; N,8,1 with no RTS/CTS
	U1STA = 0;				// Default all status bits
	U1STAbits.UTXEN = 1;	// Enable
	
	return 0;
}

// ============================================================================
//
// uputc -- Send a character to the UART. Busy wait until it is ready
//
int putcUart1( int ch )
{
	while ( U1STAbits.UTXBF == 1 )
		;

	U1TXREG = ch;

	return ch;
}

// ============================================================================
//
// ugetc -- Fetch a character from the UART, return -1 if none available
//
int getcUart1( void )
{
	if ( 1 == U1STAbits.URXDA )
		return U1RXREG;
	else
		return -1;
}

// ============================================================================
