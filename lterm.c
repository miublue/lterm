#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <vte/vte.h>
#include <stdio.h>
#include "config.h"

#define TERM_KEY(k) (event->keyval == (k) && modifiers == (MOD))
static char *cmd[32] = { "/bin/bash", NULL };
static char *dir, *xid, *title = "lterm", *font = FONT;
static double font_scale = 1, alpha = ALPHA;
static GdkRGBA background, palette[16];
static GtkWidget *window, *term;

static void setup_terminal(VteTerminal *term) {
    PangoFontDescription *pfont = pango_font_description_from_string(font);
    for (int i = 0; i < 16; ++i)
        gdk_rgba_parse(palette+i, colors[i]);
    background = palette[0];
    background.alpha = alpha;

    vte_terminal_spawn_async(term, VTE_PTY_DEFAULT, dir, cmd, NULL,
        G_SPAWN_DEFAULT, NULL, NULL, NULL, -1, NULL, NULL, NULL);
    vte_terminal_set_cursor_blink_mode(term, VTE_CURSOR_BLINK_OFF);
    vte_terminal_set_colors(term, &palette[15], &background, palette, 16);
    vte_terminal_set_font(term, pfont);
    vte_terminal_set_font_scale(term, font_scale);

    GdkScreen *s = gtk_widget_get_screen(window);
    gtk_widget_set_visual(window, gdk_screen_get_rgba_visual(s));
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &background);
}

static gboolean keypress(GtkWidget *w, GdkEventKey *event) {
    GdkModifierType modifiers = event->state & gtk_accelerator_get_default_mod_mask();
    if (TERM_KEY(GDK_KEY_C))
        vte_terminal_copy_clipboard_format(VTE_TERMINAL(term), VTE_FORMAT_TEXT);
    else if (TERM_KEY(GDK_KEY_V))
        vte_terminal_paste_clipboard(VTE_TERMINAL(term));
    else if (TERM_KEY(GDK_KEY_plus))
        vte_terminal_set_font_scale(VTE_TERMINAL(term), (font_scale += 0.1));
    else if (TERM_KEY(GDK_KEY_underscore))
        vte_terminal_set_font_scale(VTE_TERMINAL(term), (font_scale -= 0.1));
    else if (TERM_KEY(GDK_KEY_BackSpace))
        vte_terminal_set_font_scale(VTE_TERMINAL(term), (font_scale = 1.0));
    else return FALSE;
    return TRUE;
}

void main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h")) {
            printf("usage: %s [-h|-w xid|-d dir|-t title|-f font|-a alpha] [command [args ...]]\n", argv[0]);
            return;
        } else if (!strcmp(argv[i], "-d")) {
            dir = argv[++i];
        } else if (!strcmp(argv[i], "-w")) {
            xid = argv[++i];
        } else if (!strcmp(argv[i], "-t")) {
            title = argv[++i];
        } else if (!strcmp(argv[i], "-f")) {
            font = argv[++i];
        } else if (!strcmp(argv[i], "-a")) {
            alpha = strtod(argv[++i], NULL);
            alpha = (alpha < 0? 0 : alpha > 1? 1 : alpha);
        } else {
            cmd[1] = "-c";
            for (int j = i; j < argc; ++j)
                cmd[j-i+2] = argv[i];
            break;
        }
    }

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    term = vte_terminal_new();
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 540);
    gtk_window_set_title(GTK_WINDOW(window), title);
    g_signal_connect(window, "key-press-event", G_CALLBACK(keypress), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(term, "child-exited", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(window), term);
    setup_terminal(VTE_TERMINAL(term));
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(term);

    if (xid) {
        Window p = strtol(xid, NULL, 0);
        Display *d = gdk_x11_display_get_xdisplay(gdk_display_get_default());
        Window w = gdk_x11_window_get_xid(gtk_widget_get_window(window));
        XReparentWindow(d, w, p, 0, 0);
    }
    gtk_main();
}
