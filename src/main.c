#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <gtk/gtk.h>

static gboolean ok = false;
static GtkWindow *window = NULL;
static GtkWidget
        *password_entry = NULL,
        *ok_button = NULL,
        *cancel_button = NULL,
        *yes_no_button = NULL,
        *yes_button = NULL,
        *no_button = NULL,
        *other_button = NULL,
        *box = NULL,
        *prompt_label = NULL,
        *ok_cancel_box = NULL,
        *yes_no_box = NULL;
static char *window_title = "Input";
static char *prompt_text = "Password:";
static GtkApplication *app = NULL;
static gboolean has_yes_no = false, is_yes_no = false;

static void confirm_func() {
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(password_entry));
	const char *password = gtk_entry_buffer_get_text(buffer);
	g_print("%s\n", password);
	ok = true;
	gtk_window_close(GTK_WINDOW(window));
}

static void cancel_func() {
	ok = false;
	gtk_window_close(GTK_WINDOW(window));
}

static void yes_func() {
	g_print("yes\n");
	ok = true;
	gtk_window_close(GTK_WINDOW(window));
}

static void no_func() {
	g_print("no\n");
	ok = true;
	gtk_window_close(GTK_WINDOW(window));
}

static void update_buttons(void) {
	if (!has_yes_no) is_yes_no = false;
	gtk_widget_set_visible(yes_no_button, has_yes_no);  // Show Yes/No button if we can switch
	gtk_widget_set_visible(password_entry, !is_yes_no); // Hide password entry in Yes/No dialog
	gtk_widget_set_visible(ok_cancel_box, !is_yes_no);  // Hide OK/Cancel buttons in Yes/No dialog
	gtk_widget_set_visible(yes_no_box, is_yes_no);      // Show Yes/No buttons in Yes/No dialog
}

static void other_func() {
	is_yes_no = false;
	update_buttons();
}

static void yes_no_func() {
	is_yes_no = true;
	update_buttons();
}

static void activate_func() {
	confirm_func();
}

static void set_all_equal_size(gboolean hexpand, gboolean vexpand, GtkWidget **widgets) {
	GtkRequisition size;
	int width = 0, height = 0;
	for (GtkWidget **widget = widgets; *widget; ++widget) {
		gtk_widget_get_preferred_size(*widget, &size, NULL);
		if (size.width > width) width = size.width;
		if (size.height > height) height = size.height;
	}
	for (GtkWidget **widget = widgets; *widget; ++widget) {
		gtk_widget_set_hexpand(*widget, hexpand);
		gtk_widget_set_vexpand(*widget, vexpand);
		gtk_widget_set_size_request(*widget, width, height);
	}
}

static void update_text(void) {
	if (window) {
		// Set window title
		gtk_window_set_title(window, window_title);
	}
	if (prompt_label) {
		// Set prompt text and visibility accordingly
		gtk_label_set_text(GTK_LABEL(prompt_label), prompt_text ? prompt_text : "");
		gtk_widget_set_visible(prompt_label, prompt_text && *prompt_text);
	}
}

static void set_up_window(void) {
	// Main window
	window = GTK_WINDOW(gtk_application_window_new(app));
	gtk_window_set_default_size(window, 200, 0);

	// Set up widgets
	password_entry = gtk_entry_new();
	ok_button = gtk_button_new_with_label("OK");
	cancel_button = gtk_button_new_with_label("Cancel");
	yes_no_button = gtk_button_new_with_label("Yes/No");
	yes_button = gtk_button_new_with_label("Yes");
	no_button = gtk_button_new_with_label("No");
	other_button = gtk_button_new_with_label("Other");

	// Password entry
	gtk_entry_set_visibility(GTK_ENTRY(password_entry), false);
	gtk_entry_set_input_purpose(GTK_ENTRY(password_entry), GTK_INPUT_PURPOSE_PASSWORD);

	// Set button sizes
	set_all_equal_size(true, false, (GtkWidget *[]){ok_button, cancel_button, yes_no_button, yes_button, no_button, other_button, NULL});

	// Window layout
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); // 10px spacing between widgets
	gtk_widget_set_margin_start(box, 10);            // 10px margin between all edges
	gtk_widget_set_margin_end(box, 10);
	gtk_widget_set_margin_top(box, 10);
	gtk_widget_set_margin_bottom(box, 10);

	prompt_label = gtk_label_new(prompt_text); // Prompt label
	gtk_label_set_xalign(GTK_LABEL(prompt_label), 0.0);

	// Add widgets to box
	gtk_box_append(GTK_BOX(box), prompt_label);
	gtk_box_append(GTK_BOX(box), password_entry);

	// Box for OK and Cancel buttons
	ok_cancel_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_append(GTK_BOX(ok_cancel_box), yes_no_button);
	gtk_box_append(GTK_BOX(ok_cancel_box), cancel_button);
	gtk_box_append(GTK_BOX(ok_cancel_box), ok_button);

	// Box for Yes and No buttons
	yes_no_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_append(GTK_BOX(yes_no_box), other_button);
	gtk_box_append(GTK_BOX(yes_no_box), no_button);
	gtk_box_append(GTK_BOX(yes_no_box), yes_button);

	// Add boxes to main box
	gtk_box_append(GTK_BOX(box), ok_cancel_box);
	gtk_box_append(GTK_BOX(box), yes_no_box);

	// Set main box as window child
	gtk_window_set_child(GTK_WINDOW(window), box);

	// When user presses enter on password entry
	g_signal_connect(password_entry, "activate", G_CALLBACK(activate_func), window);

	// When user presses button
	g_signal_connect(ok_button, "clicked", G_CALLBACK(confirm_func), window);
	g_signal_connect(cancel_button, "clicked", G_CALLBACK(cancel_func), window);
	g_signal_connect(yes_no_button, "clicked", G_CALLBACK(yes_no_func), window);
	g_signal_connect(yes_button, "clicked", G_CALLBACK(yes_func), window);
	g_signal_connect(no_button, "clicked", G_CALLBACK(no_func), window);
	g_signal_connect(other_button, "clicked", G_CALLBACK(other_func), window);

	update_buttons();
	update_text();

	// TODO: allow resizing horizontally, but not vertically
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	gtk_window_present(window);
}

bool detect_yes_no(const char *string) {
	// match strings like "yes/no" or "no, yes"
#define QUOTE "[\"'`’‘”“]?" // all kinds of quotes
#define SPACE "\\s*"        // any amount of whitespace
#define SPACE_START "(?:^|\\s*)"
#define SPACE_END "(?:\\s*|$)"
#define SPACE_QUOTE SPACE QUOTE SPACE
#define SPACE_QUOTE_START SPACE_START QUOTE SPACE
#define SPACE_QUOTE_END SPACE SPACE_QUOTE SPACE_END
#define REGEX_Q(a, b) \
	"(?:" SPACE_QUOTE_START a SPACE_QUOTE "[,\\/|:;]" SPACE_QUOTE b SPACE_QUOTE_END ")"
#define REGEX REGEX_Q("yes", "no") "\x7c" REGEX_Q("no", "yes") "|(?:\\?" SPACE "(\\n|\\r|$))" // or match a question mark at the end

	int rc;
	PCRE2_SIZE erroroffset;
	PCRE2_SPTR pattern = (PCRE2_SPTR) REGEX;
	pcre2_code *regex = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_CASELESS | PCRE2_UTF, &rc, &erroroffset, NULL);
	if (!regex) {
		PCRE2_UCHAR8 buffer[256];
		pcre2_get_error_message(rc, buffer, sizeof(buffer));
		g_error("PCRE2 compilation failed at offset %d: %s\n", (int) erroroffset, buffer);
		return false;
	}

	pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regex, NULL);
	pcre2_code_free(regex);
	if (!match_data) g_error("PCRE2 match data creation failed\n");

	rc = pcre2_match(regex, (PCRE2_SPTR8) string, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);
	pcre2_match_data_free(match_data);

	if (rc == PCRE2_ERROR_NOMATCH) {
		has_yes_no = false;
		goto end;
	} else if (rc < 0) {
		PCRE2_UCHAR8 buffer[256];
		pcre2_get_error_message(rc, buffer, sizeof(buffer));
		g_error("PCRE2 matching failed: %s\n", buffer);
		return false;
	}

	has_yes_no = true;
end:
	is_yes_no = has_yes_no;
	return true;
}

static gint command_line(GApplication *app, GApplicationCommandLine *cmdline, gpointer user_data) {
	int argc;
	gchar **arguments = g_application_command_line_get_arguments(cmdline, &argc);

	// set up window title and prompt
	if (argc == 2) {
		prompt_text = g_strdup(arguments[1]);
	} else if (argc > 2) {
		window_title = g_strdup(arguments[1]);
		prompt_text = g_strdup(arguments[2]);
	} else
		goto end;

	g_strfreev(arguments);

	if (!window_title) g_error("Failed to get window title\n");
	if (!prompt_text) g_error("Failed to get prompt text\n");

	if (!detect_yes_no(prompt_text)) return 1;

end:
	set_up_window();

	return 0;
}

int main(int argc, char *argv[]) {
	app = gtk_application_new("me.mekb.askpass", G_APPLICATION_DEFAULT_FLAGS | G_APPLICATION_NON_UNIQUE | G_APPLICATION_HANDLES_COMMAND_LINE);
	g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	if (status != 0) return status;
	return ok ? 0 : 1;
}
