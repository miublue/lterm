LIB = `pkg-config --libs --cflags gtk+-3.0 vte-2.91 gdk-x11-3.0 x11`
all:
	cc -O2 -o lterm *.c ${LIB}

install: all
	install lterm /usr/local/bin
