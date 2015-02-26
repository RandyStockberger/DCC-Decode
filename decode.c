/* 
 * File:   decode.c
 * Author: randy
 *
 * Created on February 25, 2015, 11:17 AM
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

#define FCPU	16000000UL
#define T2PERIOD	0xFFFF

volatile uint32_t dly;
volatile uint16_t TimerTick;
long CountTime;

void __attribute__((__interrupt__, __auto_psv__))_T2Interrupt(void)
{
	IFS0bits.T2IF = 0;		// Clear interrupt flag
	++TimerTick;
}

int main(int argc, char** argv)
{
	// Configure PORTA to drive LED
    TRISAbits.TRISA4 = 0;       // Bit 4 is output

	CLKDIVbits.RCDIV = 0;
	
	// Configure Timer2 for heartbeat interrupt, timer will run to its full count
	// Generate an interrupt on overflow and use it to drive heartbeat time.
	// This timer will also be used for input capture timing.
	T2CON = 0;				// Stop timer and reset control bits
	TMR2 = 0;
	PR2 = T2PERIOD;			// Load maximum period
	IPC1bits.T2IP = 7;		// Priority level low
	IFS0bits.T2IF = 0;		// Clear any stale interrupt
	IEC0bits.T2IE = 1;		// Enable Timer2 interrupts
	T2CONbits.TON = 1;		// Start Timer2

	// Main Loop
    while(1) {
		if ( TimerTick ) {
			--TimerTick;
			CountTime -= T2PERIOD;
			if ( CountTime <= T2PERIOD ) {
				CountTime += FCPU;
				LATAbits.LATA4 ^= 1;
			}
		}
    }
    return 0;
}

