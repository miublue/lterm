#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <vte/vte.h>
#include <stdio.h>
#include "config.h"

#define CMD_MAX 255
#define TERM_KEY(k) (event->keyval == (k) && modifiers == (MOD))
static char *cmd[CMD_MAX] = { SHELL, NULL };
static char *opt_dir, *opt_wid, *opt_title = TITLE, *opt_font = FONT;
static double font_scale = 1, opt_alpha = ALPHA;
static GdkRGBA background, palette[16];
static GtkWidget *window, *term;

static void set_alpha_scale(double scale) {
    background.alpha = scale;
    vte_terminal_set_colors(VTE_TERMINAL(term), &palette[15], &background, palette, 16);
}

static void setup_terminal(VteTerminal *term) {
    PangoFontDescription *font = pango_font_description_from_string(opt_font);
    for (int i = 0; i < 16; ++i)
        gdk_rgba_parse(palette+i, colors[i]);
    background = palette[0];
    background.alpha = opt_alpha;

    vte_terminal_spawn_async(term, VTE_PTY_DEFAULT, opt_dir, cmd, NULL,
        G_SPAWN_DEFAULT, NULL, NULL, NULL, -1, NULL, NULL, NULL);
    vte_terminal_set_cursor_blink_mode(term, VTE_CURSOR_BLINK_OFF);
    vte_terminal_set_font(term, font);
    vte_terminal_set_font_scale(term, font_scale);

    GdkScreen *s = gtk_widget_get_screen(window);
    gtk_widget_set_visual(window, gdk_screen_get_rgba_visual(s));
}

static gboolean cb_key_press(GtkWidget *w, GdkEventKey *event) {
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
    else if (TERM_KEY(GDK_KEY_less))
        set_alpha_scale(background.alpha > 0? background.alpha - 0.05 : 0);
    else if (TERM_KEY(GDK_KEY_greater))
        set_alpha_scale(background.alpha < 1? background.alpha + 0.05 : 1);
    else if (TERM_KEY(GDK_KEY_question))
        set_alpha_scale(opt_alpha);
    else return FALSE;
    return TRUE;
}

void usage(char *prg) {
    printf("usage: %s [-h|-w wid|-d dir|-t title|-f font|-a alpha] [command [args ...]]\n", prg);
    printf("    -h        show help\n");
    printf("    -w wid    launch terminal within another X11 window\n");
    printf("    -d dir    launch terminal in specified directory\n");
    printf("    -t title  set specified window title\n");
    printf("    -f font   set specified font\n");
    printf("    -a alpha  set window transparency from 0 to 1\n");
    exit(0);
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h") || (argv[i][0] == '-' && argc < 3)) {
            usage(argv[0]);
        } else if (!strcmp(argv[i], "-d")) {
            opt_dir = argv[++i];
        } else if (!strcmp(argv[i], "-w")) {
            opt_wid = argv[++i];
        } else if (!strcmp(argv[i], "-t")) {
            opt_title = argv[++i];
        } else if (!strcmp(argv[i], "-f")) {
            opt_font = argv[++i];
        } else if (!strcmp(argv[i], "-a")) {
            opt_alpha = strtod(argv[++i], NULL);
            opt_alpha = (opt_alpha < 0? 0 : opt_alpha > 1? 1 : opt_alpha);
        } else {
            cmd[1] = "-c";
            for (int j = i; j < argc; ++j) {
                cmd[j-i+2] = argv[j];
            }
            break;
        }
    }

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    term = vte_terminal_new();
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), opt_title);
    g_signal_connect(window, "key-press-event", G_CALLBACK(cb_key_press), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(term, "child-exited", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(window), term);
    setup_terminal(VTE_TERMINAL(term));
    set_alpha_scale(opt_alpha);
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(term);

    if (opt_wid) {
        Window p = strtol(opt_wid, NULL, 0);
        Display *d = gdk_x11_display_get_xdisplay(gdk_display_get_default());
        Window w = gdk_x11_window_get_xid(gtk_widget_get_window(window));
        XReparentWindow(d, w, p, 0, 0);
    }
    gtk_main();
    return 0;
}
