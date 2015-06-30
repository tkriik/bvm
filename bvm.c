#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bvm.h"

#define BVM_CYCLE	1000	/* BVM cycle interval in microseconds. */
#define BVM_REGSIZE	12	/* BVM register count. */
#define BVM_MEMSIZE	256	/* BVM memory size. */

/* BVM type definition. */
struct _bvm {
	uint8_t	reg[BVM_REGSIZE];	/* Register file. */
	uint8_t	mem[BVM_MEMSIZE];	/* Memory. */
	size_t	clk;			/* Cycle clock. */
};

/* Allocates a new BVM instance. */
bvm_t *
bvm_new(void)
{
	bvm_t *vm = calloc(1, sizeof(*vm));
	return vm;
}

/* Increment a BVM instance's program counter while returning
 * current value it's pointing to. */
static inline uint8_t
pc_next(bvm_t *vm)
{
	return vm->mem[vm->reg[PC]++];
}

#define IS_REGISTER_VALID(r) ((r) < BVM_REGSIZE)

/* BVM condition code shifts. */
enum {
	CC_EQ_SHIFT	= 0,
	CC_OF_SHIFT	= 1,
	CC_EOF_SHIFT	= 2
};

/* BVM condition codes. */
enum {
	CC_EQ	= 1 << CC_EQ_SHIFT,	/* Equality flag. */
	CC_OF	= 1 << CC_OF_SHIFT,	/* Overflow flag. */
	CC_EOF	= 1 << CC_EOF_SHIFT	/* End-of-file flag. */
};

/* BVM condition code macros. */
#define IS_EQ(x)	((x) & CC_EQ)
#define IS_LT(x) 	((x) & CC_OF)
#define IS_GT(x) 	((x) ^ CC_OF && (x) ^ CC_EQ)
#define IS_EOF(x)	((x) & CC_EOF)

/* Executes a program in a BVM instance until termination.
 * Returns a non-zero error code if an error occurred. */
int
bvm_run(bvm_t *vm, const uint8_t *code, size_t code_size)
{
	/* Code size must fit into BVM's memory. */
	if (code_size > sizeof(vm->mem))
		return BVM_PROGRAM_TOO_LARGE;

	/* Zero register file and memory. */
	memset(vm->reg, 0, sizeof(vm->reg));
	memset(vm->mem, 0, sizeof(vm->mem));

	/* Load program code into memory. */
	for (size_t i = 0; i < code_size; i++)
		vm->mem[i] = code[i];

	/* Reset the clock. */
	vm->clk = 0;

	while (1) {
		usleep(BVM_CYCLE);
		vm->clk++;

		uint8_t op = pc_next(vm);
		uint8_t src, dst;
		int c;

		switch (op) {
		case HALT:
			return 0;

		case MOV:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] = vm->reg[src];
			continue;

		case MOVN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] = src;
			continue;

		case LOAD:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] = vm->mem[vm->reg[src]];
			continue;

		case LOADA:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] = vm->mem[src];
			continue;

		case PUT:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->mem[vm->reg[dst]] = vm->reg[src];
			continue;

		case PUTN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->mem[vm->reg[dst]] = src;
			continue;

		case PUTA:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->mem[dst] = vm->reg[src];
			continue;

		case PUTNA:
			src = pc_next(vm);
			dst = pc_next(vm);
			vm->mem[dst] = src;
			continue;

		case PUSH:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[SP]--;
			vm->mem[vm->reg[SP]] = vm->reg[src];
			continue;

		case PUSHN:
			src = pc_next(vm);
			vm->reg[SP]--;
			vm->mem[vm->reg[SP]] = src;
			continue;

		case POP:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[src] = vm->mem[vm->reg[SP]];
			vm->reg[SP]++;
			continue;

		case INC:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[src]++;
			continue;

		case DEC:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[src]--;
			continue;

		case ADD:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] += vm->reg[src];
			continue;

		case ADDN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] += src;
			continue;

		case SUB:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] -= vm->reg[src];
			continue;

		case SUBN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] -= src;
			continue;

		case AND:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] &= vm->reg[src];
			continue;

		case ANDN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] &= src;
			continue;

		case OR:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] |= vm->reg[src];
			continue;

		case ORN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] |= src;
			continue;

		case XOR:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] ^= vm->reg[src];
			continue;

		case XORN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[dst] ^= src;
			continue;

		case NEG:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[src] = ~vm->reg[src];
			continue;

		case CMP:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src) || !IS_REGISTER_VALID(dst))
				return BVM_BAD_INSTRUCTION;
			vm->reg[CC]  = 0;
			vm->reg[CC] |= (vm->reg[src] == vm->reg[dst]) << CC_EQ_SHIFT;
			vm->reg[CC] |= (vm->reg[src]  < vm->reg[dst]) << CC_OF_SHIFT;
			continue;

		case CMPN:
			src = pc_next(vm);
			dst = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[CC]  = 0;
			vm->reg[CC] |= (vm->reg[src] == dst) << CC_EQ_SHIFT;
			vm->reg[CC] |= (vm->reg[src]  < dst) << CC_OF_SHIFT;
			continue;

		case TEST:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			vm->reg[CC]  = 0;
			vm->reg[CC] |= (vm->reg[src] == 0) << CC_EQ_SHIFT;
			continue;

		case JMP:
			vm->reg[PC] = pc_next(vm);
			continue;

		case JE:
			src = pc_next(vm);
			if (IS_EQ(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JNE:
			src = pc_next(vm);
			if (!IS_EQ(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JL:
			src = pc_next(vm);
			if (IS_LT(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JLE:
			src = pc_next(vm);
			if (IS_LT(vm->reg[CC]) || IS_EQ(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JG:
			src = pc_next(vm);
			if (!IS_LT(vm->reg[CC]) && !IS_EQ(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JGE:
			src = pc_next(vm);
			if (!IS_LT(vm->reg[CC]) || IS_EQ(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JEOF:
			src = pc_next(vm);
			if (IS_EOF(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case JNEOF:
			src = pc_next(vm);
			if (!IS_EOF(vm->reg[CC]))
				vm->reg[PC] = src;
			continue;

		case CALL:
			vm->reg[SP]--;
			vm->mem[vm->reg[SP]] = vm->reg[PC] + 1;
			vm->reg[PC] = pc_next(vm);
			continue;

		case RET:
			vm->reg[PC] = vm->mem[vm->reg[SP]];
			vm->reg[SP]++;
			continue;

		case NOP:
			continue;

		case IN:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			if ((c = fgetc(stdin)) == EOF)
				vm->reg[CC] = CC_EOF;
			vm->reg[src] = c;
			continue;

		case OUT:
			src = pc_next(vm);
			if (!IS_REGISTER_VALID(src))
				return BVM_BAD_INSTRUCTION;
			if (fputc(vm->reg[src], stdout) == EOF)
				vm->reg[CC] = CC_EOF;
			continue;

		case OUTN:
			src = pc_next(vm);
			if (fputc(src, stdout) == EOF)
				vm->reg[CC] = CC_EOF;
			continue;

		default:
			return BVM_BAD_INSTRUCTION;
		}
	}
}

/* Returns a BVM instance's cycle clock value. */
size_t
bvm_clock(bvm_t *vm)
{
	return vm->clk;
}

/* Dumps a BVM instance's register file and memory to standard error output. */
void
bvm_dump(bvm_t *vm)
{
	for (size_t i = 0; i < BVM_MEMSIZE; i += 16) {
		fprintf(stderr, "0x%02x: ", (unsigned int)i);

		for (size_t j = 0; j < 16; j++) {
			fprintf(stderr, " %02x", vm->mem[i + j]);
			if (j == 7)
				fputc(' ', stderr);
		}

		fprintf(stderr, "  |");
		for (size_t j = 0; j < 16; j++) {
			char c = vm->mem[i + j];
			fputc(isprint(c) ? c : '.', stderr);
		}
		fprintf(stderr, "|\n");
	}
}
