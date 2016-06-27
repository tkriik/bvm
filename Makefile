CC=
CFLAGS=		-g -ansi -pedantic -Wall -Wextra -D_DEFAULT_SOURCE

SRC=		main.c bvm.c
BIN=		bvm

all:
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)
