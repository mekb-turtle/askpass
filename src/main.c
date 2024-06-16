#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <gtk/gtk.h>

static bool ok = false;
static GtkWindow *window;
static GtkWidget *password_entry;
static char *window_title = "Enter Password";
static GtkApplication *app;

static void ok_button_func() {
	ok = true;
	gtk_window_close(GTK_WINDOW(window));
}

static void cancel_button_func() {
	ok = false;
	gtk_window_close(GTK_WINDOW(window));
}

static void set_up_window() {
	window = GTK_WINDOW(gtk_application_window_new(app));
	gtk_window_set_title(window, window_title);
	gtk_window_set_default_size(window, 200, 200);

	password_entry = gtk_entry_new();
	GtkWidget *ok_button = gtk_button_new_with_label("OK");
	GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");

	gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
	g_signal_connect(ok_button, "clicked", G_CALLBACK(ok_button_func), window);
	g_signal_connect(cancel_button, "clicked", G_CALLBACK(cancel_button_func), window);
    gtk_widget_set_hexpand(ok_button, TRUE);
    gtk_widget_set_hexpand(cancel_button, TRUE);

	GtkRequisition min_size_1, min_size_2;
	gtk_widget_get_preferred_size(ok_button, &min_size_1, NULL);
	gtk_widget_get_preferred_size(cancel_button, &min_size_2, NULL);
	int max_width = MAX(min_size_1.width, min_size_2.width);
	int max_height = MAX(min_size_1.height, min_size_2.height);
	gtk_widget_set_size_request(ok_button, max_width, max_height);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_margin_start(box, 10);
	gtk_widget_set_margin_end(box, 10);
	gtk_widget_set_margin_top(box, 10);
	gtk_widget_set_margin_bottom(box, 10);

	GtkWidget *label = gtk_label_new("Password:");
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);

	gtk_box_append(GTK_BOX(box), label);
	gtk_box_append(GTK_BOX(box), password_entry);
	GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_append(GTK_BOX(button_box), cancel_button);
	gtk_box_append(GTK_BOX(button_box), ok_button);
	gtk_box_append(GTK_BOX(box), button_box);

	gtk_window_set_child(GTK_WINDOW(window), box);

	g_signal_connect(window, "close-request", G_CALLBACK(cancel_button_func), NULL);

	gtk_window_present(window);
}

static void command_line(GApplication *app, GApplicationCommandLine *cmdline, gpointer user_data) {
	// get first argument and set it as window title
	gchar **arguments = g_application_command_line_get_arguments(cmdline, NULL);
	window_title = g_strdup(arguments[1]);
	g_strfreev(arguments);
	
	gtk_window_set_title(window, window_title);
}

int main(int argc, char *argv[]) {
	int status;

	app = gtk_application_new("me.mekb.askpass", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(set_up_window), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	if (!ok) status = 1;

	return status;
}
