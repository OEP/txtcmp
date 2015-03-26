CFLAGS = -Wall -O3

INSTALL = install -D
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 0644

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin

all: txtcmp

install:
	$(INSTALL) txtcmp $(DESTDIR)$(bindir)/txtcmp

uninstall:
	rm -f $(DESTDIR)$(bindir)/txtcmp

clean:
	rm -f txtcmp

test: txtcmp
	$(MAKE) -C t

prove: txtcmp
	$(MAKE) -C t prove
