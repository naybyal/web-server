#include <gtk/gtk.h>
#include <pthread.h>

// Forward declaration for server function
void start_server();

GtkWidget *start_button;
GtkWidget *stop_button;
GtkWidget *status_label;
pthread_t server_thread;
int server_running = 0;

void *server_thread_func(void *arg) {
    start_server();
    return NULL;
}

void start_server_cb(GtkWidget *widget, gpointer data) {
    if (!server_running) {
        server_running = 1;
        pthread_create(&server_thread, NULL, server_thread_func, NULL);
        gtk_label_set_text(GTK_LABEL(status_label), "Server Status: Running");
        gtk_widget_set_sensitive(start_button, FALSE);
        gtk_widget_set_sensitive(stop_button, TRUE);
    }
}

void stop_server_cb(GtkWidget *widget, gpointer data) {
    if (server_running) {
        server_running = 0;
        close(server_fd);  // Close server socket to stop the server
        pthread_cancel(server_thread);  // Stop the server thread
        gtk_label_set_text(GTK_LABEL(status_label), "Server Status: Stopped");
        gtk_widget_set_sensitive(start_button, TRUE);
        gtk_widget_set_sensitive(stop_button, FALSE);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *vbox;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Web Server Control");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_widget_set_size_request(window, 300, 200);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    status_label = gtk_label_new("Server Status: Stopped");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, TRUE, TRUE, 0);

    start_button = gtk_button_new_with_label("Start Server");
    gtk_box_pack_start(GTK_BOX(vbox), start_button, TRUE, TRUE, 0);
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_server_cb), NULL);

    stop_button = gtk_button_new_with_label("Stop Server");
    gtk_box_pack_start(GTK_BOX(vbox), stop_button, TRUE, TRUE, 0);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(stop_server_cb), NULL);
    gtk_widget_set_sensitive(stop_button, FALSE);  // Initially disabled

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

