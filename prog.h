#ifndef _PROG_H_
#define _PROG_H_

const uint8_t PROG[] = {
	IN,	R0,
	JEOF,	10,
	OUT,	R0,
	OUTN,	' ',
	JMP,	0,
	HALT
};

#endif
