
#ifndef __DECODE_H__
#define __DECODE_H__

//#define FOSC      16000000UL
//#define FOSC      14745600L           // Oscillator frequency
//#define FOSC        7372800L          // Crystal frequency
#define FOSC        (7372800L*4L)       // Crystal frequency

#define TIMER_PRE   8L                  // Timer divisor from TxCONbits.TCKPS
#define HB_HZ       100L                // Heartbeat interrupt frequency
#define FCY         ((FOSC)/2L)         // CPU Cycle speed is 1/2 oscillator frequency
#define FT2         ((FCY)/TIMER_PRE)   // Heartbeat timer frequency
#define HB_TICKS    (FT2)/(HB_HZ)       // Heartbeat timer period

#define BAUDRATE    115200L             // UART speed
//
// ============================================================================
//
// Decoder timing
//
#if 0
#define DCC_ONE_MIN	(52U*T1_TICS_PER_US)
#define DCC_ONE_MAX	(64U*T1_TICS_PER_US)
#define DCC_ZERO_MIN	(90U*T1_TICS_PER_US)
#define DCC_ZERO_MAX	(10000U*T1_TICS_PER_US)
#endif

//#define T2_TICS_PER_US	0.9216
#define TIMER_TICS_PER_US	0.9216

#ifdef IC_HALF_WAVE
// Values when capturing half wave signal...
#define DCC_ONE_MIN	(int)((52.0*T2_TICS_PER_US)+0.5)
#define DCC_ONE_MAX	(int)((64.0*T2_TICS_PER_US)+0.5)
#define DCC_ZERO_MIN    (int)((90.0*T2_TICS_PER_US)+0.5)
#define DCC_ZERO_MAX    (int)((10000.0*T2_TICS_PER_US)+0.5)
#else
#define DCC_ONE_MIN	(int)((2.0*52.0*TIMER_TICS_PER_US)+0.5)
#define DCC_ONE_MAX	(int)((2.0*64.0*TIMER_TICS_PER_US)+0.5)
#define DCC_ZERO_MIN    (int)((2.0*90.0*TIMER_TICS_PER_US)+0.5)
#define DCC_ZERO_MAX    (int)((10000.0*TIMER_TICS_PER_US)+(102.0)+0.5)
#endif


//
// ============================================================================
//
#ifdef __PIC24F16KA101__
#define LDSTA_TRIS	TRISAbits.TRISA4
#define LDSTA_LAT	LATAbits.LATA4
//
#define DCCIN_TRIS	TRISAbits.TRISA6
#define DCCIN_LAT	LATAbits.LATA6

// Input Capture edge selection
#define IC_EVERY	1
#define IC_FALLING	2
#define IC_RISING	3
#endif // __PIC24F16KA101__

#ifdef __PIC24FV16KM202__
#define LDSTA_TRIS	TRISBbits.TRISB5
#define LDSTA_ANSA	ANSAbits.ANSA4
#define LDSTA_LAT	LATBbits.LATB5

#define DCCIN_TRIS	TRISAbits.TRISA7
#define DCCIN_LAT	LATAbits.LATA7
//#define DCCIN_ANSA	ANSAbits.ANSA7		// No analog on A.7

// Input Capture edge selection
#define IC_RISING	1
#define IC_FALLING	2
#define IC_EVERY        3           // Every rising and falling
#endif // __PIC24FV16KM202__

extern volatile uint16_t TimerTick;
extern volatile uint16_t ICFlag;        // An Input Capture interrupt has occurred

#endif	// __DECODE_H__
