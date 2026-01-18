CC = tcc
LIBS = `pkg-config --libs --cflags gtk+-3.0 vte-2.91 gdk-x11-3.0 x11`
PREFIX = /usr/local

all:
	${CC} -O2 -o lterm *.c ${LIBS}

install: all
	mkdir -p ${PREFIX}/bin
	install -s lterm ${PREFIX}/bin

uninstall:
	rm ${PREFIX}/bin/lterm

.PHONY: all install uninstall
