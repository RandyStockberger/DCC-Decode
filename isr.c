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

// ============================================================================
//
// _T2Interrupt -- Service routine for Timer2.
//
// Does nothing except increment the TimerTick interrupt flag
//
void __attribute__((__interrupt__, __auto_psv__))_T2Interrupt(void)
{
	IFS0bits.T2IF = 0;		// Clear interrupt flag
	++TimerTick;
}

// ============================================================================
