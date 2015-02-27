/* 
** File:   decode.c
** Author: randy
**
** Created on February 25, 2015, 11:17 AM
*/

// Configuration bits
//#pragma config BWRP = OFF
//#pragma config BSS = OFF
//#pragma config GWRP = OFF
//#pragma config GCP = OFF

#pragma config FNOSC = FRCPLL
//#pragma config FNOSC = FRCDIV
//#pragma config FNOSC = FRC

//#pragma config IESO = ON
//#pragma config POSCMOD = NONE
//#pragma config OSCIOFNC = ON

//#pragma config WDTPS = PS32768
//#pragma config FWPSA = PR128
//#pragma config WINDIS = OFF
//#pragma config FWDTEN = OFF
//#pragma config BOREN = BOR0
//#pragma config PWRTEN = OFF
//#pragma config I2C1SEL = PRI
//#pragma config BORV = V27
//#pragma config MCLRE = ON
//#pragma config ICS = PGx1
//#pragma config COE = OFF
//#pragma config BKBUG = OFF
//#pragma config DSWDTPS = DSWDTPSF
//#pragma config DSWDTOSC = LPRC
//#pragma config RTCOSC = SOSC
//#pragma config DSBOREN = OFF
//#pragma config DSWDTEN = OFF

//#include <stdio.h>
//#include <stdlib.h>
#include <stdint.h>
#include <p24fxxxx.h>

#include "decode.h"
#include "uart.h"

// Timer2 period register value
#define T2TICKS	0xFFFF

volatile uint16_t TimerTick;
long CountTime;

// ============================================================================
//
// InitTimer2 -- Initialize Timer2
//
// Generate a heartbeat interrupt and provide the time base for DCC bit timing
// via the input capture interrupt
//
// Requires a _T2Interrupt() ISR routine
//
void InitTimer2( void )
{
	T2CON = 0;				// Stop timer and reset control bits
	TMR2 = 0;
	PR2 = T2TICKS;			// Load maximum period
	IPC1bits.T2IP = 7;		// Priority level low
	IFS0bits.T2IF = 0;		// Clear any stale interrupt
	IEC0bits.T2IE = 1;		// Enable Timer2 interrupts
	T2CONbits.TON = 1;		// Start Timer2
}

// ============================================================================
//
void uputnibblehex( uint16_t bval )
{
	uputc( "0123456789ABCDEF"[bval] );
}

// ============================================================================
//
void uputbytehex( uint16_t bval )
{
	uputnibblehex( (bval >> 4) & 0x000F );
	uputnibblehex( bval & 0x000F );
}

// ============================================================================
//
void uputwordhex( uint16_t val )
{
	uputbytehex( (val >> 8) & 0x00FF );
	uputbytehex( val & 0x00FF );
}
// ============================================================================
//
void ShowBaud( uint16_t baud )
{
	int idx;

	// Print a sequence to sync up with terminal
	// Print CR LF
	// Print current Baud divisor
	// Print CR LF
	for ( idx=0; idx<10; ++idx )
		uputc('=');
	uputc(' ');
	uputc('0');
	uputc('x');
	uputwordhex( baud );
	uputc('\r');
	uputc('\n');
}

#ifdef rlsDEBUG
#define UBRGMIN	(BRGDIVISOR)-5
#define UBRGMAX	(BRGDIVISOR)+5
#endif
// ============================================================================
//
int main(int argc, char** argv)
{
#ifdef rlsDEBUG
	int ubrg = UBRGMIN;
#endif	// rlsDEBUG
	int LoopCount = 0;
	int ch = ' ';

	AD1PCFG = 0xFFFF;			// Disable all analog channels
	CLKDIVbits.RCDIV = 0;		// Set primary oscillator speed

	// Configure PORTA to drive LED
    TRISAbits.TRISA4 = 0;       // PORTB.4 is output
	LATAbits.LATA4 = 0;			// Turn of the LED

	// Configure timer2 to drive heartbeat interrupt
	InitTimer2();
	InitUart1();
#ifdef rlsDEBUG
U1BRG = ubrg;
#endif	// rlsDEBUG

	// Main Loop
    while(1) {
		if ( TimerTick ) {
			--TimerTick;
			CountTime -= T2TICKS;
			if ( CountTime <= T2TICKS ) {
				++LoopCount;
				CountTime += FCPU;

#if rlsDEBUG
				ShowBaud( U1BRG );
				if ( 0 == (LoopCount & 0x0007) ) {
					if ( ubrg < UBRGMAX ) {
						++ubrg;
						U1BRG = ubrg;
					}
				}
#endif	// rlsDEBUG
				LATAbits.LATA4 ^= 1;
			}
		}
#if 1
		uputc(++ch);
		if ( ch >= 0x7E )
			ch = ' ';
#endif
	}
    return 0;
}

// ============================================================================
