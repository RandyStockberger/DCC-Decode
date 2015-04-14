//
// DccState.c
//
//  Created on: Feb 4, 2014
//      Author: randy
//
//
//=====================================================================
//
//  Preamble - Series of 10 or more successive '1' bits
//  InterPacket Bit - A single bit. '0' Indicates a following data byte, '1' Indicated end of data
//  Data Byte - Eight bits of data
//  Framing Error - A bit may have invalid timing, too long or short. This forces a transition to
//  the ERROR state.
//
//  Decoding DCC packets is controlled by a state machine
//  IDLE		-- Between packets, waiting for first preamble bit
//  					'0' - Invalid preamble bit, enter ERROR
//  					'1' - Valid preamble bit, enter PREAMBLE, PreambleCount = 1
//  PREAMBLE -- Found a preamble bit, waiting for 10th preamble bit
//  					'1' - ++PreambleCount
//  						 if PreambleCount == 10 enter START
//  						 if PreambleCount < 10 remain in PREAMBLE
//  					'0' - Preamble too short, enter ERROR
//  START	-- Found 10 or more preamble bits
// 					'1' bit - ++PreambleCount, Stay in START
// 					'0' bit - Enter DATA
//  DATA		-- Receiving data bits
// 					BitCount < 8 - Stay in DATA
// 					BitCount == 8 - Save data byte and enter INTERBIT
//  INTERBIT	-- Waiting for inter data bit
//  					'0' - Expect another byte
//  						ByteCount is valid - Enter DATA
//  						ByteCount too large - Enter ERROR
//  					'1' Packet complete, Process Packet, Enter PREAMBLE with PreambleCount = 1
//  ERROR	-- Had a protocol error or illegal bit timing
// 					Process error and transition to IDLE
//
//  Preamble			- Start Bit - Data Byte - Start Bit - Data Byte - Start Bit - Data Byte - End Bit
//  11111111111111        0         10010101      0        10010100       0        11001010       1
//

#include <stdint.h>

#include "decode.h"
#include "uart.h"
#include "ustdio.h"
#include "DccState.h"

static unsigned char	Data;					// Current data byte
static enum DCCState	State;					// Current state machine state
static int				PreambleCount;			// Number of preamble bits seen
static unsigned int		ByteCount;				// Number of valid data bytes in this packet
static int				BitCount;				// Number of bits seen in current data byte
static unsigned char	DataBuf[DCC_MAX_BYTES];	// Packet data buffer

//=====================================================================
//
// DCC Data
//	0 bit:	52uS .. 64uS, nominal is 58uS
//	1 bit:	90uS .. 10,000uS
//
//=====================================================================

// ====================================================================
//
// ProcessDCCBuffer -- Do something with the bytes in the DCC Packet
//
void ProcessDCCBuffer( void )
{
	int idx;

#ifdef UNFINISHED
	if ( 0 == (DataBuf[0] & 0x80) ) {
		uprintf("%d ", DataBuf[0] & 0x7F);
		uputc( ((DataBuf[1] & 0x40) == 1) ? 'F' : 'R');
#endif	// UNFINISHED
	for ( idx=0; idx<ByteCount; ++idx ) {
		uprintf(" %x", DataBuf[idx] );
	}
	uprintf("\r\n");
}

//=====================================================================
//
// DCC_Idle -- Enter the DCC_IDLE state
//
void DCC_Idle( void )
{
	Data = 0;
	State = DCC_IDLE;
	PreambleCount = ByteCount = BitCount = 0;
}

//=====================================================================
//
// DoError -- Recover from a data stream error
//
void DoError( const enum DCCError Error )
{
	//enum DCCError { ERR_NONE, ERR_TIMING, ERR_PREAMBLE, ERR_BYTECOUNT };
	switch( Error ) {
	case ERR_TIMING:
		if ( State != DCC_IDLE ) {		// Do nothing if already in the IDLE state
			DCC_Idle();
			uputc('V');
		}
		break;
	case ERR_PREAMBLE:
		DCC_Idle();
//		uputc('P');						// Don't flag errors durring preamble processing
		break;
	case ERR_BYTECOUNT:
		DCC_Idle();
		uputc('#');
		break;
	case ERR_NONE:
		uputc('?');
		break;
	}
}

//=====================================================================
//
// ProcessBit -- Run the new bit thru the DCC State machine
//
void ProcessBit( unsigned int bit )
{
	switch( State ) {
	case 	DCC_IDLE:
		if ( 1 == bit ) {
			State = DCC_PREAMBLE;
			PreambleCount = 1;
		}
		else {
			// Already in DCC_IDLE, do nothing...
			//DoError( ERR_PREAMBLE );				// Display error and transition to IDLE
		}
		break;
	case	DCC_PREAMBLE:
		if ( 1 == bit ) {
			if ( ++PreambleCount >= 10 ) {
				// Still accumulating preamble bits but a '0' is now a valid start of data
				State = DCC_START;
			}
		}
		else {
//			uprintf("%d",PreambleCount);
			DoError( ERR_PREAMBLE );
		}
		break;
	case	DCC_START:
		++PreambleCount;
		if ( 0 == bit ) {
			State = DCC_DATA;
			Data = 0;
			BitCount = ByteCount = 0;
		}
		break;
	case	DCC_DATA:
		Data <<= 1;
		Data |= bit;
		if ( ++BitCount >= 8 ) {
			State = DCC_INTERBIT;
			if ( ByteCount < DCC_MAX_BYTES )
				DataBuf[ByteCount] = Data;
			++ByteCount;
		}
		break;
	case	DCC_INTERBIT:
		if ( 1 == bit ) {		// Packet Complete
			if ( (ByteCount >= DCC_MIN_BYTES) && (ByteCount <= DCC_MAX_BYTES) ) {
				ProcessDCCBuffer();
				State = DCC_PREAMBLE;
				PreambleCount = 1;
				BitCount = ByteCount = 0;
			}
			else {
				DoError( ERR_BYTECOUNT );				// Display error and transition to IDLE
			}
		}
		if ( 0 == bit ) {		// Expect another data byte
			State = DCC_DATA;
			BitCount = 0;
			Data = 0;
		}
		break;
	default:
		break;
	}
}

// ====================================================================
//
// ProcessDCCBit -- Cycle the DCC state machine using the new bit as input
//
void ProcessDCCBit( unsigned int BitTime )
{
	if ( BitTime <= DCC_ONE_MAX && BitTime >= DCC_ONE_MIN ) {
		ProcessBit( 1 );
	}
	else if ( BitTime <= DCC_ZERO_MAX && BitTime >= DCC_ZERO_MIN ) {
		ProcessBit( 0 );
	}
	else {
		DoError( ERR_TIMING );
	}

}

// ====================================================================


