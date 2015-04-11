/* 
** File:   isr.c
** Author: randy
**
** Created February, 2015
**
** Interrupt Service routines for DCC-Decode program
**
*/

#include <stdint.h>
#include <p24fxxxx.h>

#include "decode.h"

#ifdef __PIC24F16KA101__
// ============================================================================
//
// _T2Interrupt -- Service routine for Heartbeat Timer.
//
// Does nothing except increment the TimerTick flag
//
void __attribute__((__interrupt__, __auto_psv__))_T2Interrupt(void)
{
	IFS0bits.T2IF = 0;		// Clear interrupt flag
	++TimerTick;
}
#endif	// __PIC24F16KA101__

#ifdef __PIC24FV16KM202__
// ============================================================================
//
// _T1Interrupt -- Service routine for Heartbeat Timer.
//
// Does nothing except increment the TimerTick flag
//
void __attribute__((__interrupt__, __auto_psv__))_T1Interrupt(void)
{
	IFS0bits.T1IF = 0;		// Clear interrupt flag

	++TimerTick;
}
#endif	// __PIC24FV16KM202__

#ifdef ENABLED

// ============================================================================
//
// _IC1Interrupt -- Service routine for Timer2.
//
// Some splainin to do
//
#ifdef __PIC24F16KA101__
void __attribute__((__interrupt__, __auto_psv__))_IC1Interrupt(void)
{
	IFS0bits.IC1IF = 0;		// Clear interrupt flag
	++ICFlag;
}
#endif

#ifdef __PIC24FV16KM202__
	// TODO -- Add ISR routine
#endif

// ============================================================================

#endif // ENABLED

