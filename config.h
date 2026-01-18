#ifndef _CONFIG_H
#define _CONFIG_H

#define MOD GDK_CONTROL_MASK|GDK_SHIFT_MASK
#define TITLE "Terminal"
#define FONT "Terminess Nerd Font 25"
#define ALPHA 1.0
#define SHELL "/bin/mksh"
#define WIDTH 900
#define HEIGHT 540

const char *colors[16] = {
    "#000000", "#ff5faf", "#5fffaf", "#ffaf5f", "#87d7ff", "#ff5faf", "#87d7ff", "#eeeeee",
    "#808080", "#ff5faf", "#5fffaf", "#ffaf5f", "#87d7ff", "#ff5faf", "#87d7ff", "#eeeeee",
};

#endif
