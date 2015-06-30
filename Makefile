CC=		cc
CFLAGS=		-g -std=gnu99 -Wall -Wextra

HEADERS=	bvm.h
SOURCES=	main.c bvm.c
BVM=		bvm

all:
	$(CC) $(CFLAGS) -o $(BVM) $(SOURCES)
