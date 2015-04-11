#ifndef UART_H
#define	UART_H

/*
** uart.h
**
** Methods and Structures that actually talk to the UART
*/
// The provided formula rounds down. This can result in a non-optimal error percentage
// Divisor = ( 16,000,000 / ( 16 * 115200 ) ) - 1
// Divisor = 7.68, which sould round to 8 to minimize the error rate
//
// FCPU: 16,000,000; Baud Rate: 115,200
// Calculated Divisor: 7
// Actual Baud Rate with divisor of 7: 125,000 baud. Error is 7.8%
// Actual Baud Rate with divisor of 8: 111,111 baud. Error is 3.5%
//
// FCPU: 16,000,000; Baud Rate: 57,600
// Calculated Divisor: 16
// Actual Baud Rate with divisor of 16: 58,823 baud. Difference: 1,123. Error is 7.8%
// Actual Baud Rate with divisor of 17: 55,555 baud. Difference: 2,005. Error is 3.5%
//

//#define BRGDIVISOR          3
#define BRGDIVISOR (int)((FCY / (16UL * BAUDRATE)) - 1UL)

extern int InitUart1( void );
extern int putcUart1( int ch );
extern int getcUart1( void );

#define uputc(ch) putcUart1(ch)
#define ugetc(ch) getcUart1(ch)

#endif	/* UART_H */

