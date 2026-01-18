# lterm
Tiny vte terminal emulator that i made for fun.

## Installing
Compile with:
```sh
make install
```

## Usage
```
lterm [-h|-w wid|-d dir|-t title|-f font|-a alpha] [command [args ...]]
    -h        show help
    -w wid    launch terminal within another X11 window
    -d dir    launch terminal in specified directory
    -t title  set specified window title
    -f font   set specified font
    -a alpha  set window transparency from 0 to 1
```

## Configuring
* Simply edit `config.h` and recompile.

