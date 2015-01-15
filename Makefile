CFLAGS = -Wall -O3

all: txtcmp

test: txtcmp
	./txtcmp txtcmp.c txtcmp.c Makefile
