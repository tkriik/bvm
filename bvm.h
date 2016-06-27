#ifndef _BVM_H_
#define _BVM_H_

#include <stddef.h>
#include <stdint.h>

/* BVM opcodes. */
enum {
	NOP,	/* No-op. */

	MOV,	/* Register-to-register move. */
	MOVN,	/* Value-to-register move. */
	LOAD,	/* Value from address in register to register. */
	LOADA,	/* Value from address to register. */
	PUT,	/* Value from register to address in register. */
	PUTN,	/* Value to address in register. */
	PUTA,	/* Value from register to address in register. */
	PUTNA,	/* Value to address. */
	PUSH,	/* Value from register to top of stack. */
	PUSHN,	/* Value to top of stack. */
	POP,	/* Value from top of stack to register. */

	INC,	/* Increment register value. */
	DEC,	/* Decrement register value. */
	ADD,	/* Add source register to destination register. */
	ADDN,	/* Add source value to destination register. */
	SUB,	/* Subtract source register from destination register. */
	SUBN,	/* Subtract source value from destination register. */

	AND,	/* Bitwise AND between source register and destination register. */
	ANDN,	/* Bitwise AND between source value and destination register. */
	OR,	/* Bitwise OR between source register and destination register. */
	ORN,	/* Bitwise OR between source value and destination register. */
	XOR,	/* Bitwise XOR between source register and destination register. */
	XORN,	/* Bitwise XOR between source value and destination register. */
	NEG,	/* Bitwise negation of a register. */

	CMP,	/* Compare source register to destination register .*/
	CMPN,	/* Compare source register to destination value. */
	TEST,	/* Compare source register to zero. */

	JMP,	/* Unconditional jump to address. */
	JE,	/* Jump to address if last comparison was equal. */
	JNE,	/* Jump to address if last comparison was not equal. */
	JL,	/* Jump to address if last comparison was less. */
	JLE,	/* Jump to address if last comparison was less or equal. */
	JG,	/* Jump to address if last comparison was greater. */
	JGE,	/* Jump to address if last comparison was greater or equal. */
	JEOF,	/* Jump to address if last I/O returned EOF. */
	JNEOF,	/* Jump to address if last I/O did not return EOF. */

	CALL,	/* Push the PC on stack and jump to address. */
	RET,	/* Pop a value from stack and 'return' to it. */

	IN,	/* Read a byte from standard input. */
	OUT,	/* Write a value from register to standard output. */
	OUTN,	/* Write a value to standard output. */

	HALT	/* Stop program execution. */
};

/* BVM registers. */
enum {
	PC,		/* Program counter. */
	SP,		/* Stack pointer. */
	CC,		/* Condition codes. */
	XR,		/* Index register. */
	R0, R1, R2, R3,	/* General-purpose registers. */
	R4, R5, R6, R7
};

#define BVM_REGSIZE 12	/* BVM register count. */
#define BVM_MEMSIZE 256 /* BVM memory size. */

/* bvm structure. */
struct bvm {
	uint8_t	reg[BVM_REGSIZE];	/* Register file. */
	uint8_t	mem[BVM_MEMSIZE];	/* Memory. */
	size_t	clk;			/* Cycle clock. */
};

/* BVM errors. */
enum {
	BVM_HALT,		/* Program terminated successfully. (no error) */
	BVM_BAD_INSTRUCTION,	/* Invalid instruction encountered. */
	BVM_PROGRAM_TOO_LARGE	/* Size of program to be executed was too big. */
};

/* BVM routines. */
int	 bvm_run(struct bvm *, const uint8_t *, size_t);
size_t	 bvm_clock(struct bvm *);
void	 bvm_dump(struct bvm *);

#endif
