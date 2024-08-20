#include <gtk/gtk.h>
#include <vte/vte.h>
#include "config.h"

#define TERM_KEY(k) (event->keyval == (k) && modifiers == (MOD))
char *cmd[32] = { "/bin/bash", NULL };
GtkWindow *window;
GtkWidget *term;
GdkRGBA palette[16];
GdkRGBA background;
PangoFontDescription *font;
double font_scale = 1;

void setup_terminal(VteTerminal *term) {
    font = pango_font_description_from_string(FONT);
    for (int i = 0; i < 16; ++i)
        gdk_rgba_parse(palette+i, colors[i]);
    background = palette[0];
    background.alpha = ALPHA;

    vte_terminal_spawn_async(term, VTE_PTY_DEFAULT, NULL, cmd, NULL,
        G_SPAWN_DEFAULT, NULL, NULL, NULL, -1, NULL, NULL, NULL);
    vte_terminal_set_cursor_blink_mode(term, VTE_CURSOR_BLINK_OFF);
    vte_terminal_set_colors(term, &palette[15], &background, palette, 16);
    vte_terminal_set_font(term, font);
    vte_terminal_set_font_scale(term, font_scale);
    g_signal_connect(term, "child-exited", G_CALLBACK(gtk_main_quit), NULL);
}

void setup_visual() {
    GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(window));
    gtk_widget_set_visual(GTK_WIDGET(window), gdk_screen_get_rgba_visual(screen));
    gtk_widget_override_background_color(GTK_WIDGET(window), GTK_STATE_FLAG_NORMAL, &background);
}

gboolean keypress(GtkWidget *widget, GdkEventKey *event) {
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
    if (argc > 2 && !strcmp(argv[1], "-c")) {
        for (int i = 1; i < argc; ++i)
            cmd[i] = argv[i];
    }

    window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_default_size(window, 900, 540);
    g_signal_connect(GTK_WIDGET(window), "key-press-event", G_CALLBACK(keypress), NULL);
    g_signal_connect(GTK_WIDGET(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    term = vte_terminal_new();
    gtk_container_add(GTK_CONTAINER(window), term);
    setup_terminal(VTE_TERMINAL(term));
    setup_visual();
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_widget_grab_focus(term);
    gtk_main();
}
