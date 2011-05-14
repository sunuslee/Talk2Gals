#include <gtk/gtk.h>
int main(int agrc, char *argv[])
{
        GtkWidget *button_enter;
        GtkWidget *button_client;
        GtkWidget *button_server;
        GtkWidget *table;
        GtkWidget *window;

        gtk_init(&argc, &argv);
        
        window = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(window), "talk2gals");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);

        gtk_widget_show(window);
        gtk_main();
        return 0;
}
