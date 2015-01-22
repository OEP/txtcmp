CFLAGS = -Wall -O3

all: txtcmp

clean:
	rm -f txtcmp

test: txtcmp
	$(MAKE) -C t

prove: txtcmp
	$(MAKE) -C t prove
