#include <stdint.h>
#include <stdio.h>

#include "bvm.h"

int
main(void)
{
	/* Disable buffering on standard output. */
	setbuf(stdout, NULL);

	const uint8_t program[] = {
		IN,	R0,
		JEOF,	22,
		CMPN,	R0,	'\t',
		JE,	0,
		CMPN,	R0,	'\n',
		JE,	0,
		CMPN,	R0,	' ',
		JE,	0,
		OUT,	R0,
		JMP,	0,
		HALT
	};

	bvm_t *vm = bvm_new();

	fprintf(stderr, "Running program... (%zu bytes)\n", sizeof(program));
	switch (bvm_run(vm, program, sizeof(program))) {
	case BVM_BAD_INSTRUCTION:
		fprintf(stderr, "Illegal instruction.\n");
		bvm_dump(vm);
		return -1;
	case BVM_PROGRAM_TOO_LARGE:
		fprintf(stderr, "Program size too large.\n");
		return -1;
	}

	fprintf(stderr, "Program finished with %zu instructions.\n", bvm_clock(vm));
	bvm_dump(vm);

	return 0;
}
