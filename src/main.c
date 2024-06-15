#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <gtk/gtk.h>

static bool ok = false;
static GtkWindow *window, *password_entry;
static char *window_title;

static void ok_button_func() {
	ok = true;
	gtk_window_close(GTK_WINDOW(window));
}

static void cancel_button_func() {
	ok = false;
	gtk_window_close(GTK_WINDOW(window));
}

static void activate(GtkApplication *app, gpointer user_data) {
	window = GTK_WINDOW(gtk_application_window_new(app));
	gtk_window_set_title(window, window_title);
	gtk_window_set_default_size(window, 200, 200);

	password_entry = gtk_entry_new();
	GtkWidget *ok_button = gtk_button_new_with_label("OK");
	GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");

	gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
	g_signal_connect(ok_button, "clicked", G_CALLBACK(ok_button_func), window);
	g_signal_connect(cancel_button, "clicked", G_CALLBACK(cancel_button_func), window);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_margin_start(box, 10);
	gtk_widget_set_margin_end(box, 10);
	gtk_widget_set_margin_top(box, 10);
	gtk_widget_set_margin_bottom(box, 10);

	GtkWidget *label = gtk_label_new("Password:");
    gtk_widget_set_hexpand(label, FALSE);

	gtk_box_append(GTK_BOX(box), label);
	gtk_box_append(GTK_BOX(box), password_entry);
	gtk_box_append(GTK_BOX(box), cancel_button);
	gtk_box_append(GTK_BOX(box), ok_button);

	gtk_window_set_child(GTK_WINDOW(window), box);

	g_signal_connect(window, "close-request", G_CALLBACK(cancel_button_func), NULL);

	gtk_window_present(window);
}

static void command_line(GApplication *app, GApplicationCommandLine *cmdline, gpointer user_data) {
	g_application_command_line_set_exit_status(cmdline, ok ? 0 : 1);
}

int main(int argc, char *argv[]) {
	GtkApplication *app;
	int status;

	app = gtk_application_new("me.mekb.askpass", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	if (!ok) status = 1;

	return status;
}
