#include <stdint.h>
#include <stdio.h>

#include "bvm.h"
#include "prog.h"

int
main(void)
{
	struct bvm vm;

	setbuf(stdout, NULL);

	fprintf(stderr, "Running program... (%d bytes)\n", (int)sizeof(PROG));

	switch (bvm_run(&vm, PROG, sizeof(PROG))) {
	case BVM_BAD_INSTRUCTION:
		fprintf(stderr, "Illegal instruction.\n");
		bvm_dump(&vm);
		return -1;
	case BVM_PROGRAM_TOO_LARGE:
		fprintf(stderr, "Program size too large.\n");
		return -1;
	}

	bvm_dump(&vm);

	return 0;
}
