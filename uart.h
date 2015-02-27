/* 
** uart.h
**
*/
#ifndef UART_H
#define	UART_H

#define BRGDIVISOR (int)((FCPU / (16UL * BAUDRATE)) - 1UL)
//#define BRGDIVISOR (int)(8)

extern int InitUart1( void );
extern int uputc( int ch );
extern int ugetc( void );
extern int uputs( char * pStr );

#endif	/* UART_H */

