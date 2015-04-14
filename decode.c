/* 
** decode.c
**
** February, 2015
**
*/

// Configuration bits
//#pragma config BWRP = OFF
//#pragma config BSS = OFF
//#pragma config GWRP = OFF
//#pragma config GCP = OFF

//#pragma config FNOSC = FRCPLL
//#pragma config FNOSC = FRCDIV
//#pragma config FNOSC = FRC
//#pragma config FNOSC = PRI			// Primary oscillator, XT, HS or EC
#pragma config FNOSC = PRIPLL		// Primary oscillator, XT, HS or EC with PLL

//#pragma config IESO = ON			// Internal clock to external clock switchover
//#pragma config POSCMOD = NONE
#pragma config POSCMOD = HS			// Primary Oscillator is HS, between 4MHz and 25MHz
#pragma config POSCFREQ = MS		// Oscillator freq between 100kHz and 8MHz
#pragma config FCKSM = CSECMD		// Clock Switch Enabled, Clock Monitor Disabled
//#pragma config OSCIOFNC = ON

//#pragma config WDTPS = PS32768
//#pragma config FWPSA = PR128
//#pragma config WINDIS = OFF
#pragma config FWDTEN = OFF			// Watchdog timer disabled
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


/*
**			PIC24F16KA101 pin map
**
**		Pin		Port		Usage
**		 1		RA5			MCLR
**		 2		RA0
**		 3		RA1
**		 4		RB0			PGD1
**		 5		RB1			PGC1
**		 6		RB2			U1RX
**		 7		RA2			OSCI
**		 8		RA3			OSCO
**		 9		RB4
**		10		RA4			Debug LED
**		11		RB7			U1TX
**		12		RB8
**		13		RB9
**		14		RA6			IC1
**		15		RB12
**		16		RB13
**		17		RB14
**		18		RB15
**		19		VSS			GND
**		20		VDD			3.3V
**
**
*			PIC24FV16KM202 pin map
**
**		Pin		Port		Usage
**		 1		RA5			MCLR
**		 2		RA0
**		 3		RA1
**		 4		RB0			PGED1
**		 5		RB1			PGEC1
**		 6		RB2			U1RX
**		 7		RB3
**		 8		VSS			GND
**		 9		OSCI		OSCI
**		10		OSCO		OSCO
**		11		RB4
**		12		RA4
**		13		VDD			3.3V
**		14		RA5			LDSTA
**		15		RB6
**		16		RB7			U1TX
**		17		RB8
**		18		RB9
**		19		RA7			DCCIN
**		20		VCAP		VCAP
**		21		RB10
**		22		RB11
**		23		RB12
**		24		RB13
**		25		RB14
**		26		RBB15
**		27		VSS			GND
**		28		VDD			3.3V
**
*/

// ============================================================================
// Includes
#include <stdint.h>
#include <p24fxxxx.h>

#include "uart.h"
#include "ustdio.h"
#include "dccstate.h"
#include "decode.h"

// ============================================================================
// Defines
//
// ============================================================================
// Global declarations
volatile uint16_t TimerTick;
volatile uint16_t ICFlag;        // An Input Capture interrupt has occurred
//
uint16_t ICTime;
uint16_t ICTimeLast;
uint16_t DeltaTime;
//
long CountTime;
//
// ============================================================================
//
void ICSync( void );
void InitDCCCapture( void );
void InitHBTimer( void );

// ============================================================================
//
int main(int argc, char** argv)
{
	int LoopCount = 0;
	long idx;
//	int OneCount = 0;
	uint16_t uRcon;

	uRcon = RCON;			// Save reset source
	RCON = 0;				// Clear RCON for next time

	// Initial delay to let the system clocks stabilize
	for ( idx=1000; idx>0; --idx )
		;

#ifdef __PIC24F16KA101__
	AD1PCFG = 0xFFFF;			// Disable all analog channels
#endif	// __PIC24F16KA101__

	// Configure Status LED port
#ifdef __PIC24FV16KM202__
	LDSTA_ANSA = 0;				// Disable analog on LED
#endif	// __PIC24FV16KM202__
	LDSTA_TRIS = 0;				// Status LED is an output
	LDSTA_LAT = 0;				// Turn off Status LED

	// Configure IC1 pin as input
	DCCIN_TRIS = 1;				// DCCIN is an input
	//DCCIN_ANA = 0;			// DCCIN pin does not have analog

	// Configure timer2 to drive heartbeat interrupt
	InitHBTimer();
	InitUart1();
	InitDCCCapture();

	uprintf("\r\n\n\nDCC Decoder\r\nVersion 0.0.1\r\n" __DATE__ " at " __TIME__ "\r\n\n" );
	uprintf("HB_TICKS (Max timer count): %d\r\n", (int)((HB_TICKS)&0xFFFF) );
	uprintf("HZ: %d\r\n", (int)((FT2)/(HB_TICKS)) );
	uprintf("Reset register RCON:%x\r\n\r\n", uRcon );

#ifdef ICSYNC
	ICSync();
#endif // ICSYNC

	// Main Loop
    while(1) {
#ifdef USEICINTERRUPTS
		if ( ICFlag ) {
			--ICFlag;
			//LDSTA_LAT ^= 1;
		}
#endif // USEICINTERRUPTS

#ifdef __PIC24F16KA101__
		if ( IC1CONbits.ICBNE ) {
			//LATAbits.LATA4 ^= 1;
			LDSTA_LAT ^= 1;
			while ( IC1CONbits.ICBNE ) {
				ICTimeLast = ICTime;
				ICTime = IC1BUF;
				DeltaTime = ICTime - ICTimeLast;

#if 1
				if ( ICTimeLast > ICTime ) {
					DeltaTime += HB_TICKS;
				}
#else
				if ( DeltaTime & 0x8000 ) {
					DeltaTime += HB_TICKS;
				}
#endif

#if 1
				ProcessDCCBit( DeltaTime );

#else
				if ( (DeltaTime <= DCC_ONE_MAX ) && (DeltaTime >= DCC_ONE_MIN) ) {
					uputc('1');
					if ( ++OneCount > 10 ) {
						OneCount = 0;
						uputc('\r');
						uputc('\n');
					}
				}
				else if ( (DeltaTime <= DCC_ZERO_MAX) && (DeltaTime >= DCC_ZERO_MIN) ) {
					uputc('0');
					OneCount = 0;
				}
				else {
					uputc('X');
				}
#endif // 0

			}
		}
#endif // __PIC24F16KA101__
#ifdef __PIC24FV16KM202__
		// TODO: Initialize Input Capture hardware
#endif	// __PIC24FV16KM202__

		if ( TimerTick ) {
			--TimerTick;
			CountTime -= HB_TICKS;
			if ( CountTime <= HB_TICKS ) {
				++LoopCount;
				CountTime += FT2;

				LDSTA_LAT ^= 1;
//				uprintf("\r%d LoopCount - Hello", LoopCount );
//				uprintf("IC1BUF: %d\r\n", DeltaTime);
			}
		}
	}
    return 0;
}
//
// ============================================================================
//
// InitDCCCapture -- Initialize capture hardware
//
void InitDCCCapture( void ) {
#ifdef __PIC24F16KA101__
	// Configure input capture
	IC1CONbits.ICTMR = 1;		// Use Timer2
	IC1CONbits.ICI = 0;			// Interrupt on every capture event
	IC1CONbits.ICOV = 0;		// Clear possible previous capture overflow
//	IC1CONbits.ICM = 2;			// Capture every falling edge
	IC1CONbits.ICM = 1;			// Capture every edge
#ifdef USEICINTERRUPTS
	IFS0bits.IC1IF = 0;			// Clear old interrupt flag
	IPC0bits.IC1IP = 3;			// Moderate priority
	IEC0bits.IC1IE = 1;			// Enable IC1 interrupts
#endif // USEICINTERRUPTS

#endif	// __PIC24F16KA101__
#ifdef __PIC24FV16KM202__
		// TODO: Initialize Input Capture hardware
	// The PIC24FV processor has different capture hardware
	CCP1CON1L = 0;				// Return to reset state
	CCP1CON1Lbits.TMRSYNC = 1;
	CCP1CON1Lbits.TMRPS = 1;	// Clock Prescale, 0 - 1:1, 1 - 4:1, 2 - 16:1, 3 - 64:1
	CCP1CON1Lbits.CCSEL = 1;	// Capture mode
	CCP1CON1Lbits.MOD = IC_FALLING;	// Capture on falling edges

	CCP1CON1H = 0;				// Return to reset state
	CCP1CON2L = 0;				// Return to reset state
	CCP1CON2H = 0;				// Return to reset state
	CCP1CON3L = 0;				// Return to reset state
	CCP1CON3H = 0;				// Return to reset state


#endif
}
//
//
// ============================================================================
//
// InitHBTimer -- Initialize Timer2
//
// Generate a heartbeat interrupt and provide the time base for DCC bit timing
// via the input capture interrupt
//
// Requires a _T2Interrupt() ISR routine
//
void InitHBTimer( void )
{
#ifdef __PIC24F16KA101__
	T2CON = 0;				// Stop timer and reset control bits
	TMR2 = 0;
	PR2 = HB_TICKS;			// Load maximum period

	T2CONbits.TCKPS = 1;	// Period is 1/8 Fcy

	IPC1bits.T2IP = 1;		// Priority level low
	IFS0bits.T2IF = 0;		// Clear any stale interrupt
	IEC0bits.T2IE = 1;		// Enable Timer2 interrupts

	T2CONbits.TON = 1;		// Start Timer2
#endif

#ifdef __PIC24FV16KM202__
	// No Timer2 available, use Timer1
	T1CON = 0;				// Stop timer and reset control bits
	TMR1 = 0;
	PR1 = HB_TICKS;			// Timer Period

	T1CONbits.TCKPS = 1;	// 0 -- 1:1 Prescale
							// 1 -- 1:8 Prescale
							// 2 -- 1:64 Prescale
							// 3 -- 1:256 Prescale

	IPC0bits.T1IP = 1;		// Priority level low
	IFS0bits.T1IF = 0;		// Clear stale interrupt
	IEC0bits.T1IE = 1;		// Enable Timer interrupts

	T1CONbits.TON = 1;		// Start Timer2

#endif
}
#ifdef ICSYNC
//
// ============================================================================
//
// ICSync -- Analyze input capture pulses to determine correct phase
//
//	#define IC_RISING	3;
//	#define IC_FALLING	2;
//	#define IC_EVERY	1;
//
#define IC_SYNC_EDGES	10000
#define IC_TIMEOUT		100000L
#define IC_STARTEDGES	60			// Number of edges to see to assume a valid signal
void ICSync( void )
{
	static int CurrentEdge = IC_RISING;
	int	TotalEdges, TotalErrors, TotalOnes, TotalZeros;
	int TotalShort, TotalMid, TotalLong;
	unsigned long timeout;

	uprintf("Syncing DCC signal\r\n");
	uprintf("DCC_ONE_MIN: %d\r\n", DCC_ONE_MIN );
	uprintf("DCC_ONE_MAX: %d\r\n", DCC_ONE_MAX );
	uprintf("DCC_ZERO_MIN: %d\r\n", DCC_ZERO_MIN );
	uprintf("DCC_ZERO_MAX: %d\r\n", DCC_ZERO_MAX );

	TotalEdges = 0;
	while( TotalEdges < IC_STARTEDGES ) {
		uprintf("\r\nSearching for a DCC signal...");
		CurrentEdge = IC_RISING;		// Arbitrary edge to trigger on
#ifdef __PIC24F16KA101__
		IC1CONbits.ICM = CurrentEdge;
#endif
#ifdef __PIC24FV16KM202__
		// TODO: Select Current edge in capture module
#endif

		TotalEdges = 0;
		for ( timeout=IC_TIMEOUT; timeout > 0L; --timeout ) {
#ifdef __PIC24F16KA101__
			if ( 1 == IC1CONbits.ICBNE ) {
				ICTime = IC1BUF;		// Read edge time
				if ( ++TotalEdges > IC_STARTEDGES )
					break;				// Exit the for() loop
			}
#endif
#ifdef __PIC24FV16KM202__
			// TODO: Retrieve timestamp of current edge
			//	if ( 1 == IC1CONbits.ICBNE ) {
			//		ICTime = IC1BUF;		// Read edge time
			//		if ( ++TotalEdges > IC_STARTEDGES )
			//			break;				// Exit the for() loop
			//	}
#endif
		}
		uprintf("Edges:%d",TotalEdges);
	}
	uprintf("\r\nDCC signal present\r\n");


	while ( 1 ) {
		// Select edge to capture
		CurrentEdge = ( CurrentEdge == IC_RISING ) ? IC_FALLING : IC_RISING;
#ifdef __PIC24F16KA101__
		IC1CONbits.ICM = CurrentEdge;
#endif
#ifdef __PIC24FV16KM202__
			// TODO: Select edge to capture on
#endif

		// Clear statistics
		TotalEdges =  TotalErrors = TotalOnes = TotalZeros = 0;
		TotalShort = TotalMid = TotalLong = 0;

		uprintf("\r\nCapturing ");
		if ( CurrentEdge == IC_RISING )
			uprintf("RISING");
		else if ( CurrentEdge == IC_FALLING )
			uprintf("FALLING");
		else if ( CurrentEdge == IC_EVERY )
			uprintf("EVERY");
		else
			uprintf("BLARG");
		uprintf(" edges\r\n");

		while ( TotalEdges < IC_SYNC_EDGES ) {
			// Busy wait until there is Capture
			timeout = 0;
#ifdef __PIC24F16KA101__
			while ( 0 == IC1CONbits.ICBNE ) {
				if ( ++timeout > IC_TIMEOUT ) {
					break;
				}
			}
#endif
#ifdef __PIC24FV16KM202__
			// TODO: Wait for a capture
#endif

			if ( timeout > IC_TIMEOUT ) {
				DeltaTime = 0xFFFF;
			}
			else {
				ICTimeLast = ICTime;
#ifdef __PIC24F16KA101__
				ICTime = IC1BUF;
#endif
#ifdef __PIC24FV16KM202__
			// TODO: Retrieve the timestamp
#endif
				DeltaTime = ICTime - ICTimeLast;
			}

			++TotalEdges;
			if ( DeltaTime < DCC_ONE_MIN ) {
				++TotalShort;
				++TotalErrors;
			}
			else if ( DeltaTime <= DCC_ONE_MAX+7 )
				++TotalOnes;
			else if ( DeltaTime < (DCC_ZERO_MIN-7) ) {
				++TotalMid;
				++TotalErrors;
			}
			else if ( DeltaTime <= 350 )
				++TotalZeros;
			else {
				++TotalLong;
				++TotalErrors;
			}
#if 0
			if ( (DeltaTime <= DCC_ONE_MAX ) && (DeltaTime >= DCC_ONE_MIN) ) {
				//uputc('1');
				++TotalOnes;
			}
//			else if ( (DeltaTime <= DCC_ZERO_MAX) && (DeltaTime >= DCC_ZERO_MIN) ) {
			else if ( (DeltaTime <= 250) && (DeltaTime >= DCC_ZERO_MIN) ) {
				//uputc('0');
				++TotalZeros;
			}
			else {
				//uputc('X');
				++TotalErrors;
			}
#endif // 0
		}
		uprintf("TotalEdges: %d\r\n", TotalEdges );
		uprintf("TotalShort: %d\r\n", TotalShort );
		uprintf("TotalZeros: %d\r\n", TotalZeros );
		uprintf("TotalMid: %d\r\n", TotalMid );
		uprintf("TotalOnes: %d\r\n", TotalOnes );
		uprintf("TotalLong: %d\r\n", TotalLong );
		uprintf("TotalErrors: %d\r\n\n", TotalErrors );

		if ( TotalErrors < (TotalEdges/50) )
			return;
	}
}
//
//
// ============================================================================
//
#endif // ICSYNC
