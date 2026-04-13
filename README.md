# lterm
Tiny vte terminal emulator that i made for fun.

## Installing
Compile with:
```sh
make install
```

## Usage
```
lterm [ -h | -w | -d | -c | -t | -f | -a | -cw | -ch | -ww | -wh ] [command [args ...]]
    -h         show help
    -w wid     launch terminal within another X11 window
    -d dir     launch terminal in specified directory
    -c file    load colors from specified file
    -t title   set specified window title
    -f font    set specified font
    -a alpha   set window transparency from 0 to 1
    -cw scale  set terminal cell's width scale
    -ch scale  set terminal cell's height scale
    -ww width  set specified window width
    -wh height set specified window height
```

## Configuring
* Simply edit `config.h` and recompile.

