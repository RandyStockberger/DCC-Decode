/*
 * ustdio.c -- Implement pseudo stdio via system UART
 *
 *  Created on: Jan 15, 2014
 *      Author: rls
 *   Copyright: 2014, Randy Stockberger
 *
 */

#include <stdint.h>
#include <stdarg.h>

#include "uart.h"

static const char uldigits[] = "0123456789ABCDEF";
char ulbuf[12];

// ====================================================================
//
// uputs -- Send a string to the UART
//
// Return:	Non-negative value on success
//			EOF on failure
//
int uputs( char const * str )
{
	while ( *str ) {
		uputc( *str );
		++str;
	}

	return 0;
}

// ====================================================================
//
// ultoa -- Return ASCII representation of an unsigned long
//
// Return:	Pointer to a static buffer containing the resultant string
//
char *ultoa( unsigned long val, int base )
{
	char * ptr = &ulbuf[11];		// Point to end of buffer

	*ptr = '\0';

	do {
		--ptr;
		*ptr = uldigits[val % base];
		val /= base;
	} while ( val > 0 );

	return ptr;
}

// ====================================================================
//
// uprintf -- Simple formatted output after printf()
//
// Return:	Number of characters sent to output
//
// Supports format specifiers '%', 'c', 's', 'd', 'x'
void uprintf( char const *fmt, ... )
{
	va_list arg;
//	int len = 0;
	int ch;
	int val, base;
	unsigned int uval;
	char * str;
	//char buf[10];

	va_start( arg, fmt );
	while( (ch = *fmt++) ) {
		if ( '%' == ch ) {
			switch( ch = *fmt++ ) {
			case '%':			// "%%" outputs a single "%"
				uputc( ch );
				break;

			case 'c':
				uputc( va_arg( arg, int ) );
				break;

			case 's':
				str = va_arg( arg, char * );
				uputs( str );
				break;

			case 'd':
				val = va_arg( arg, int );
				if ( val < 0 ) {
					uputc( '-' );
					val = - val;
				}
				str = ultoa( (unsigned long) val, 10 );
				uputs( str );
				break;

			case 'x':
			case 'u':
				base = ( ch == 'u' ) ? 10 : 16;
				uval = va_arg( arg, unsigned int );
				str = ultoa( (unsigned long)uval, base );
				uputs( str );
				break;
			}
		}
		else {
			uputc( ch );
		}
	}
	va_end( arg );
}

// ============================================================================
