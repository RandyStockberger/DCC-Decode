#ifndef _USTDIO_H_
#define _USTDIO_H_

/*
** ustdio.h
**
** Implement a minimal input/output environment via the UART
**
*/

// Procedures to export
int uputs( char const * str );
char * ultoa( unsigned long val, int base );
void uprintf( char const *fmt, ... );

#endif /* _USTDIO_H_ */


