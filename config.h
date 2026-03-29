#ifndef _CONFIG_H
#define _CONFIG_H

#define TITLE "Terminal"
#define WIDTH 900
#define HEIGHT 540
#define ALPHA 1.0
#define FONT "Terminess Nerd Font 18"
#define SHELL "/bin/mksh"
#define HIDE_MOUSE TRUE
#define LINE_SPACING 1.2

#define KEY_COPY                  TERM_KEY(GDK_CONTROL_MASK|GDK_SHIFT_MASK, GDK_KEY_C)
#define KEY_PASTE                 TERM_KEY(GDK_CONTROL_MASK|GDK_SHIFT_MASK, GDK_KEY_V)
#define KEY_INCREMENT_FONT_SIZE   TERM_KEY(GDK_CONTROL_MASK, GDK_KEY_equal)
#define KEY_DECREMENT_FONT_SIZE   TERM_KEY(GDK_CONTROL_MASK, GDK_KEY_minus)
#define KEY_RESET_FONT_SIZE       TERM_KEY(GDK_CONTROL_MASK, GDK_KEY_0)
#define KEY_INCREMENT_OPACITY     TERM_KEY(GDK_CONTROL_MASK|GDK_SHIFT_MASK, GDK_KEY_plus)
#define KEY_DECREMENT_OPACITY     TERM_KEY(GDK_CONTROL_MASK|GDK_SHIFT_MASK, GDK_KEY_underscore)
#define KEY_RESET_OPACITY         TERM_KEY(GDK_CONTROL_MASK|GDK_SHIFT_MASK, GDK_KEY_parenright)

const char *colors[16] = {
    "#000000", "#ff5faf", "#5fffaf", "#ffaf5f", "#87d7ff", "#af87d7", "#87d7ff", "#eeeeee",
    "#808080", "#ff5faf", "#5fffaf", "#ffaf5f", "#87d7ff", "#af87d7", "#87d7ff", "#eeeeee",
};

#endif
