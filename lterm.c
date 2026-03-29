#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <vte/vte.h>
#include <stdio.h>
#include "config.h"

static struct {
    char *cmd[255];
    char *cur_dir, *x11_wid, *win_title, *font;
    double alpha_scale, line_spacing;
    int hide_mouse;
} opts;

static struct {
    GtkWidget *window, *term;
    GdkRGBA background, palette[16];
    double font_scale;
} lerm;

static void set_alpha_scale(double scale) {
    lerm.background.alpha = scale;
    vte_terminal_set_colors(VTE_TERMINAL(lerm.term), &lerm.palette[15], &lerm.background, lerm.palette, 16);
}

static void setup_terminal(VteTerminal *term) {
    PangoFontDescription *font = pango_font_description_from_string(opts.font);
    for (int i = 0; i < 16; ++i) gdk_rgba_parse(lerm.palette+i, colors[i]);
    lerm.background = lerm.palette[0];
    lerm.background.alpha = opts.alpha_scale;
    vte_terminal_spawn_async(term, VTE_PTY_DEFAULT, opts.cur_dir, opts.cmd, NULL,
        G_SPAWN_DEFAULT, NULL, NULL, NULL, -1, NULL, NULL, NULL);
    vte_terminal_set_cursor_blink_mode(term, VTE_CURSOR_BLINK_OFF);
    vte_terminal_set_font(term, font);
    vte_terminal_set_font_scale(term, 1.0);
    vte_terminal_set_enable_sixel(term, TRUE);
    vte_terminal_set_mouse_autohide(term, opts.hide_mouse);
    vte_terminal_set_cell_height_scale(term, opts.line_spacing);
    gtk_widget_set_visual(lerm.window, gdk_screen_get_rgba_visual(gtk_widget_get_screen(lerm.window)));
}

#define TERM_KEY(M, K) (event->keyval == (K) && modifiers == (M))
static gboolean cb_key_press(GtkWidget *w, GdkEventKey *event) {
    GdkModifierType modifiers = event->state & gtk_accelerator_get_default_mod_mask();
    if (KEY_COPY)
        vte_terminal_copy_clipboard_format(VTE_TERMINAL(lerm.term), VTE_FORMAT_TEXT);
    else if (KEY_PASTE)
        vte_terminal_paste_clipboard(VTE_TERMINAL(lerm.term));
    else if (KEY_DECREMENT_FONT_SIZE)
        vte_terminal_set_font_scale(VTE_TERMINAL(lerm.term), (lerm.font_scale -= 0.1));
    else if (KEY_INCREMENT_FONT_SIZE)
        vte_terminal_set_font_scale(VTE_TERMINAL(lerm.term), (lerm.font_scale += 0.1));
    else if (KEY_RESET_FONT_SIZE)
        vte_terminal_set_font_scale(VTE_TERMINAL(lerm.term), (lerm.font_scale = 1.0));
    else if (KEY_DECREMENT_OPACITY)
        set_alpha_scale(lerm.background.alpha > 0? lerm.background.alpha - 0.05 : 0);
    else if (KEY_INCREMENT_OPACITY)
        set_alpha_scale(lerm.background.alpha < 1? lerm.background.alpha + 0.05 : 1);
    else if (KEY_RESET_OPACITY)
        set_alpha_scale(opts.alpha_scale);
    else return FALSE;
    return TRUE;
}

static void usage(const char *prg) {
    printf("usage: %s [-h|-w wid|-d dir|-t title|-f font|-a alpha|-s scale] [command [args ...]]\n", prg);
    printf("    -h        show help\n");
    printf("    -w wid    launch terminal within another X11 window\n");
    printf("    -d dir    launch terminal in specified directory\n");
    printf("    -t title  set specified window title\n");
    printf("    -f font   set specified font\n");
    printf("    -a alpha  set window transparency from 0 to 1\n");
    printf("    -s scale  set line spacing\n");
    exit(0);
}

int main(int argc, char **argv) {
    opts.cmd[0] = SHELL;
    opts.win_title = TITLE, opts.font = FONT;
    opts.alpha_scale = ALPHA, opts.line_spacing = LINE_SPACING;
    opts.hide_mouse = HIDE_MOUSE;
    lerm.font_scale = 1.0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h") || (argv[i][0] == '-' && i+1 >= argc)) {
            usage(argv[0]);
        } else if (!strcmp(argv[i], "-d")) {
            opts.cur_dir = argv[++i];
        } else if (!strcmp(argv[i], "-w")) {
            opts.x11_wid = argv[++i];
        } else if (!strcmp(argv[i], "-t")) {
            opts.win_title = argv[++i];
        } else if (!strcmp(argv[i], "-f")) {
            opts.font = argv[++i];
        } else if (!strcmp(argv[i], "-a")) {
            opts.alpha_scale = strtod(argv[++i], NULL);
            opts.alpha_scale = (opts.alpha_scale < 0? 0 : opts.alpha_scale > 1? 1 : opts.alpha_scale);
        } else if (!strcmp(argv[i], "-s")) {
            opts.line_spacing = strtod(argv[++i], NULL);
        } else {
            if (argv[i][0] == '-') usage(argv[0]);
            opts.cmd[1] = "-c";
            for (int j = i; j < argc; ++j) opts.cmd[j-i+2] = argv[j];
            break;
        }
    }

    gtk_init(&argc, &argv);
    lerm.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    lerm.term = vte_terminal_new();
    gtk_window_set_default_size(GTK_WINDOW(lerm.window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW(lerm.window), opts.win_title);
    g_signal_connect(lerm.window, "key-press-event", G_CALLBACK(cb_key_press), NULL);
    g_signal_connect(lerm.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(lerm.term, "child-exited", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(lerm.window), lerm.term);
    setup_terminal(VTE_TERMINAL(lerm.term));
    set_alpha_scale(opts.alpha_scale);
    gtk_widget_show_all(lerm.window);
    gtk_widget_grab_focus(lerm.term);

    if (opts.x11_wid) {
        Window p = strtol(opts.x11_wid, NULL, 0);
        Display *d = gdk_x11_display_get_xdisplay(gdk_display_get_default());
        Window w = gdk_x11_window_get_xid(gtk_widget_get_window(lerm.window));
        XReparentWindow(d, w, p, 0, 0);
    }
    gtk_main();
    return 0;
}
