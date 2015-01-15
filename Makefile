CFLAGS = -Wall -O3

all: txtcmp

test: txtcmp
	$(MAKE) -C t

prove: txtcmp
	$(MAKE) -C t prove
