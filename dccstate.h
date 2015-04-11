
#ifndef _DCCSTATE_H_
#define	_DCCSTATE_H_
/*
** dccstate.h
**
*/

// ====================================================================
//
// Defines and definitions to support DCC decoding
//
// DCC bit timing
// "A Digital Decoder must accept bits whose first and last parts have a duration of
// between 52 and 64 microseconds as a vlaid bit with the value of '1'"
//
// "A Digital Decoder must accept bits whose first or last parts have a duration of
// between 90 and 10,000 microseconds as a valid bit with the value of '0'"
//

#define DCC_MAX_BYTES	6			// Number of bytes in longest possible packet
#define DCC_MIN_BYTES	3			// Number of bytes in shortest legal DCC packet

// ====================================================================
//
enum DCCState { DCC_IDLE, DCC_PREAMBLE, DCC_START, DCC_DATA, DCC_INTERBIT };
enum DCCError { ERR_NONE, ERR_TIMING, ERR_PREAMBLE, ERR_BYTECOUNT };

// ====================================================================
extern void ProcessDCCBit( unsigned int BitTime );


#endif	// _DCCSTATE_H_


