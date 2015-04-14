/* 
** File:   isr.c
** Author: randy
**
** Created February, 2015
**
** Interrupt Service routines for DCC-Decode program
**
** Each supported chip needs to service two interrupts, A heartbeat timer
** interrupt firing at 100..500Hz and a capture interrupt to measure timming
** of the DCC waveform.
**
*/

#include <stdint.h>
#include <p24fxxxx.h>

#include "decode.h"

#ifdef __PIC24F16KA101__
//
// Interrupts supported by the PIC24F16KA101
//
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
// ============================================================================
//
// _IC1Interrupt -- Service routine for Timer2.
//
// Some splainin to do
//
void __attribute__((__interrupt__, __auto_psv__))_IC1Interrupt(void)
{
	IFS0bits.IC1IF = 0;		// Clear interrupt flag
	++ICFlag;
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

// ============================================================================
//
// _??Interrupt -- Service routine for ?? capture interrupt
//
//void __attribute__((__interrupt__, __auto_psv__))_???Interrupt(void)
//{
//	IFS0bits.IC1IF = 0;		// Clear interrupt flag
//	++ICFlag;
//}

#endif	// __PIC24FV16KM202__

