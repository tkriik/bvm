#ifndef _BASM_H_
#define _BASM_H_

#include "bvm.h"

typedef struct {
	char	*name;
	size_t	 args;
} op_info_t;

static const op_info_t op_info[] = {
	{ .name	= "halt",	.args = 0 },
	{ .name	= "mov",	.args = 2 },
	{ .name	= "movn",	.args = 2 },
	{ .name	= "load",	.args = 2 },
	{ .name	= "loada",	.args = 2 },
	{ .name	= "put",	.args = 2 },
	{ .name	= "putn",	.args = 2 },
	{ .name	= "puta",	.args = 2 },
	{ .name	= "putna",	.args = 2 },
	{ .name	= "push",	.args = 1 },
	{ .name	= "pushn",	.args = 1 },
	{ .name	= "pop",	.args = 1 },
	{ .name	= "inc",	.args = 1 },
	{ .name	= "dec",	.args = 1 },
	{ .name	= "add",	.args = 2 },
	{ .name	= "addn",	.args = 2 },
	{ .name	= "sub",	.args = 2 },
	{ .name	= "subn",	.args = 2 },
	{ .name	= "and",	.args = 2 },
	{ .name	= "andn",	.args = 2 },
	{ .name	= "or",		.args = 2 },
	{ .name	= "orn",	.args = 2 },
	{ .name	= "xor",	.args = 2 },
	{ .name	= "xorn",	.args = 2 },
	{ .name	= "neg",	.args = 1 },
	{ .name	= "cmp",	.args = 2 },
	{ .name	= "cmpn",	.args = 2 },
	{ .name = "test",	.args = 1 },
	{ .name	= "jmp",	.args = 1 },
	{ .name	= "je",		.args = 1 },
	{ .name	= "jne",	.args = 1 },
	{ .name	= "jl",		.args = 1 },
	{ .name	= "jle",	.args = 1 },
	{ .name	= "jg",		.args = 1 },
	{ .name	= "jge",	.args = 1 },
	{ .name	= "jre",	.args = 1 },
	{ .name	= "jrne",	.args = 1 },
	{ .name	= "jrl",	.args = 1 },
	{ .name	= "jrle",	.args = 1 },
	{ .name	= "jrg",	.args = 1 },
	{ .name	= "jrge",	.args = 1 },
	{ .name = "call",	.args = 1 },
	{ .name = "ret",	.args = 0 },
	{ .name = "nop",	.args = 0 },
	{ .name = "in",		.args = 1 },
	{ .name	= "out",	.args = 1 },
	{ .name	= "outn",	.args = 1 }
};

#endif
