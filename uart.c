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
#include "uart.h"

int uinit( uint32_t baud, uint32_t fcpu )
{

	return -1;
}
int uputc( int ch )
{

	return ch;
}

int ugetc( void )
{

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