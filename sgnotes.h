#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#define ML 256

GtkIconTheme *theme;
GtkIconInfo *info;
GdkPixbuf *icon;
GtkAccelGroup *accel_group;
int wraptext;
int fix;
char *home_dir;


char *pm;
GtkWidget *dialog;
static GtkWidget *submenu_item1;
static GtkWidget *grid;
static GtkWidget *window;
static GtkWidget *list;
static GtkWidget *text_view;
static GtkWidget *textbox_grid;
static GtkWidget *save_button;
static GtkWidget *rename_button;
static GtkWidget *delete_button;
static GtkWidget *scrolled_list;
static GtkWidget *scrolled_txt;
static GtkTextBuffer *buffer;

static char current_file[1024] = ""; // Store Actual file name
char config_file_path[256];

void readconf()
{
	const char *home_dir = getenv("HOME");


	if (home_dir == NULL)
	{
		fprintf(stderr, "Error: HOME environment variable is not set.\n");
		exit(1);
	}

	snprintf(config_file_path, sizeof(config_file_path), "%s/.config/sgnotes.conf", home_dir);

	FILE *file = fopen(config_file_path, "r");

	if (file == NULL)
	{
		file = fopen(config_file_path, "w");
		if (file != NULL)
		{
			fprintf(file, "wraptext=%d\n", 1);
			wraptext = 1;
			fclose(file);
		}
		else
		{
			fprintf(stderr, "Error: Failed to open file.\n");
			exit(1);
		}
		return;
	}

	char line[ML];
	while (fgets(line, sizeof(line), file) != NULL) {
		char *name = strtok(line, "=");
		char *value_str = strtok(NULL, "=");

		if (name != NULL && value_str != NULL)
		{
			// Set the value of the corresponding variable based on the name
			if (strcmp(name, "wraptext") == 0)
			{
				wraptext = atoi(value_str);
			}
			else if (strcmp(name, "fix") == 0)
			{
				fix = atoi(value_str);
			}
		}
	}

	fclose(file);
	printf("WrapText: %d\n", wraptext);
	printf("Fix: %d\n", fix);
}





gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		GtkWidget *submenu = GTK_WIDGET(data);
		gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
		return TRUE;
	}

	return FALSE;
}

static void on_entry_changed(GtkEditable *editable, gpointer user_data)
{
	GtkWidget *entry = GTK_WIDGET(editable);
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
	gchar *cleaned_text = g_strdelimit(g_strdup(text), "/\\", '\0');
	gtk_entry_set_text(GTK_ENTRY(entry), cleaned_text);
	g_free(cleaned_text);
}


void save_file_content()
{
	//Fix saving empty files
	if (current_file[0] == '\0')
	{
		return;
	}

	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char file_path[1024];
	snprintf(file_path, sizeof(file_path), "%s%s%s", home, notes_dir, current_file);

	FILE *file = fopen(file_path, "w");
	if (file)
	{
		GtkTextIter start, end;
		gtk_text_buffer_get_start_iter(buffer, &start);
		gtk_text_buffer_get_end_iter(buffer, &end);
		gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		fprintf(file, "%s", text);
		fclose(file);
		g_free(text);
	}
}

void on_save_button_clicked(GtkButton *button, gpointer user_data)
{
	save_file_content();
}


void restart_program(GtkWidget *widget, gpointer data)
{
	printf("Program Reloaded...\n");
	char *args[] = {pm, NULL};
	execvp(args[0], args);
}

void load_file_list()
{
	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char notes_path[1024];
	snprintf(notes_path, sizeof(notes_path), "%s%s", home, notes_dir);

	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(notes_path)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_REG) // Disable showing directories
			{
				gtk_list_box_insert(GTK_LIST_BOX(list), gtk_label_new(entry->d_name), -1);
			}
		}
		closedir(dir);
	}
}

void clear_file_list()
{
	GList *children, *iter;
	children = gtk_container_get_children(GTK_CONTAINER(list));
	for (iter = children; iter != NULL; iter = g_list_next(iter))
	{
		GtkWidget *row = GTK_WIDGET(iter->data);
		gtk_container_remove(GTK_CONTAINER(list), row);
	}
	g_list_free(children);
}

void delfromlist(GtkListBox *list, GtkListBoxRow *row)
{
	gtk_container_remove(GTK_CONTAINER(list), GTK_WIDGET(row));
	gtk_widget_destroy(GTK_WIDGET(row));
}

void delete_current_file()
{
	if (current_file[0] == '\0')
	{
		return;
	}

	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char file_path[1024];
	snprintf(file_path, sizeof(file_path), "%s%s%s", home, notes_dir, current_file);

	if (remove(file_path) == 0)
	{
		gtk_text_buffer_set_text(buffer, "", -1);
		current_file[0] = '\0';

		GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list));
		if (selected_row != NULL)
		{
			delfromlist(GTK_LIST_BOX(list), selected_row);
		}
	}
	else
	{
		// Error handler
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Can't delete file: '%s'", current_file);
		gtk_window_set_position(GTK_WINDOW(error_dialog), GTK_WIN_POS_CENTER);
		gtk_window_set_title(GTK_WINDOW(error_dialog), "Error deleting file");
		gtk_dialog_run(GTK_DIALOG(error_dialog));
		gtk_widget_destroy(error_dialog);
	}
}

void on_delete_button_clicked(GtkButton *button, gpointer user_data)
{
	if (current_file[0] == '\0')
	{
		return;
	}

	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure you want to delete this note: '%s'?", current_file);
	gtk_window_set_title(GTK_WINDOW(dialog), "WARNING");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	if (response == GTK_RESPONSE_YES)
	{
		delete_current_file();
	}
}

void load_file_content(const char *filename)
{
	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char file_path[1024];
	snprintf(file_path, sizeof(file_path), "%s%s%s", home, notes_dir, filename);

	FILE *file = fopen(file_path, "r");
	if (file)
	{
		gtk_text_buffer_set_text(buffer, "", -1);

		char *line = NULL;
		size_t len = 0;
		ssize_t read;

		while ((read = getline(&line, &len, file)) != -1)
		{
			gtk_text_buffer_insert_at_cursor(buffer, line, read);
		}

		fclose(file);

		if (line)
		{
			free(line);
		}

		strncpy(current_file, filename, sizeof(current_file));
	}
}



void on_list_item_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data)
{
	GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
	const char *filename = gtk_label_get_text(GTK_LABEL(label));
	gtk_widget_show(textbox_grid);
	gtk_widget_show(scrolled_txt);
	load_file_content(filename);
}



static void save_file(const gchar *filename)
{
	gchar *full_path;
	FILE *file;

	const char *home_dir = g_get_home_dir();

	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", NULL);
	if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
		g_mkdir_with_parents(dir_path, 0700);

	full_path = g_build_filename(dir_path, filename, NULL);
	file = fopen(full_path, "w");
	if (file)
	{
		fclose(file);
		g_free(full_path);
	}
	else
	{
		g_print("can't create file.\n");
	}

	g_free(dir_path);
}

void additem(GtkButton *button, GtkListBox *list, const gchar *text)
{
	GtkWidget *label = gtk_label_new(text);
	gtk_list_box_insert(GTK_LIST_BOX(list), label, -1);
	gtk_widget_show_all(GTK_WIDGET(list));
}

void saveToFile(const gchar *text)
{
	gchar *full_path;
	FILE *file;

	const char *home_dir = g_get_home_dir();

	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", NULL);
	if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
		g_mkdir_with_parents(dir_path, 0700);

	full_path = g_build_filename(dir_path, text, NULL);

	// Check if the file already exists
	if (g_file_test(full_path, G_FILE_TEST_EXISTS))
	{
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(NULL,
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_YES_NO,
			"The file already exists. Do you want to overwrite it?");
		gtk_window_set_title(GTK_WINDOW(dialog), "Confirm");
		gint result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		if (result != GTK_RESPONSE_YES)
		{
			g_free(full_path);
			g_free(dir_path);
			return; // Do not overwrite the existing file
		}
	}

	file = fopen(full_path, "w");
	if (file)
	{
		fclose(file);
		g_print("Note saved to %s\n", full_path);
		additem(GTK_BUTTON(submenu_item1), GTK_LIST_BOX(list), text);
		g_free(full_path);
	}
	else
	{
		g_print("Unable to create the file.\n");
				// Error handler
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Unable to create the file: \n '%s'", text);
		gtk_window_set_title(GTK_WINDOW(error_dialog), "Unable to create the file.");
		gtk_dialog_run(GTK_DIALOG(error_dialog));
		gtk_widget_destroy(error_dialog);
	}

	g_free(dir_path);

}


static void on_rename_button_clicked(GtkButton *button, gpointer user_data)
{
	if (current_file[0] == '\0')
	{
		return;
	}

	const gchar *text = "";
	gchar *output;
	gchar *input;
	FILE *file;

	GtkWidget *dialog;
		GtkWidget *content_area;
		GtkWidget *entry;
		GtkWidget *label;
		GtkWidget *ok_button;
		GtkWidget *cancel_button;
	dialog = gtk_dialog_new_with_buttons("Rename Note:",
		GTK_WINDOW(user_data),
		GTK_DIALOG_MODAL,
		"OK",
		GTK_RESPONSE_OK,
		"Cancel", GTK_RESPONSE_CANCEL, NULL);
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

		gtk_widget_set_size_request(dialog, 333, -1);
		content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

		entry = gtk_entry_new();
		gtk_container_add(GTK_CONTAINER(content_area), entry);
		g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(on_entry_changed), dialog);
		gtk_widget_show_all(dialog);

	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result == GTK_RESPONSE_OK)
	{
		text = gtk_entry_get_text(GTK_ENTRY(entry));
		const char *home_dir = g_get_home_dir();

		gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", NULL);
		if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
			g_mkdir_with_parents(dir_path, 0700);
		output = g_build_filename(dir_path, text, NULL);

		// Check if the file already exists
		if (g_file_test(output, G_FILE_TEST_EXISTS))
		{
			GtkWidget *dialog;
			gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
				dialog = gtk_message_dialog_new(NULL,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
			"The file already exists. Do you want to overwrite it?");
			gtk_window_set_title(GTK_WINDOW(dialog), "Confirm");
			gint result = gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);

			if (result != GTK_RESPONSE_YES)
			{
				g_free(output);
				g_free(dir_path);
				return;
			}
		}
		input = g_build_filename(dir_path, current_file, NULL);
		strcpy(current_file, text);
		if (rename(input, output) == 0)
		{
			printf("File moved succesfully.\n");
				GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list));
		if (selected_row != NULL)
		{
			GtkWidget *row_widget = gtk_bin_get_child(GTK_BIN(selected_row));
			if (GTK_IS_LABEL(row_widget))
			{
				gtk_label_set_text(GTK_LABEL(row_widget), text);
			}
		}
		else
		{
			perror("Error moving file");
		}
}

	}
	else if (result == GTK_RESPONSE_CANCEL)
	{
		g_print("\n");
	}
	gtk_widget_destroy(dialog);
}

static void on_submenu_item1_selected(GtkWidget *widget, gpointer data)
{
GtkWidget *dialog;
	GtkWidget *content_area;
	GtkWidget *entry;
	GtkWidget *label;
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	dialog = gtk_dialog_new_with_buttons("New Note:",
	GTK_WINDOW(data),
	GTK_DIALOG_MODAL,
	"OK",
	GTK_RESPONSE_OK,
	"Cancel", GTK_RESPONSE_CANCEL, NULL);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

	gtk_widget_set_size_request(dialog, 333, -1);
	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(content_area), entry);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(on_entry_changed), dialog);
	gtk_widget_show_all(dialog);

	gint result = gtk_dialog_run(GTK_DIALOG(dialog));

	if (result == GTK_RESPONSE_OK)
	{
		const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
		saveToFile(text);
	}
	else if (result == GTK_RESPONSE_CANCEL)
	{
		g_print("\n");
	}
	gtk_widget_destroy(dialog);
}

static void on_submenu_item2_selected(GtkWidget *widget, gpointer data)
{
	const char *home_dir = getenv("HOME");
	char config_file_path[256];
	if (home_dir == NULL) {
		fprintf(stderr, "Error: HOME environment variable is not set.\n");
		exit(1);
	}


	snprintf(config_file_path, sizeof(config_file_path), "%s/.config/sgnotes.conf", home_dir);

	FILE *config_file = fopen(config_file_path, "r");
	if (config_file == NULL) {
		wraptext = 0;
	} else {
		char line[256];
		while (fgets(line, sizeof(line), config_file)) {
			if (strstr(line, "wraptext=") == line) {

				int value;
				if (sscanf(line, "wraptext=%d", &value) == 1) {
					wraptext = value;
				}
				break;
			}
		}
		fclose(config_file);
	}

	if (wraptext == 0) {
		wraptext = 1;
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
	} else if (wraptext == 1) {
		wraptext = 0;
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
	}


	config_file = fopen(config_file_path, "w");
	if (config_file != NULL) {
		fprintf(config_file, "wraptext=%d\n", wraptext);
		fclose(config_file);
	} else {
		fprintf(stderr, "Error: Failed to open file.\n");
	}
}

void on_submenu_item3_selected(GtkMenuItem *menuitem, gpointer userdata)
{
	dialog = gtk_about_dialog_new();

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SGNotes");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Copyright © 2023 ItzSelenux for Simple GTK Desktop Environment");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Simple GTK Launcher");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://sgde.github.io/sgnotes");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), "Project WebSite");
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog),GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(dialog),"accessories-notes");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

gboolean timeout_callback(gpointer user_data)
{
	on_save_button_clicked(NULL, user_data);
	return G_SOURCE_CONTINUE;
}
