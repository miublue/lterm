OUT = lterm
LIB = `pkg-config --libs --cflags gtk+-3.0 vte-2.91`
all:
	gcc -o $(OUT) *.c $(LIB) -O2

install: all
	install $(OUT) /usr/local/bin
