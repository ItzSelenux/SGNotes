#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/stat.h>
#define ML 256

GtkIconTheme *theme;
GtkIconInfo *info;
GdkPixbuf *icon;
GtkAccelGroup *accel_group;
int wraptext;
int fix;
char *pm;
GtkWidget *dialog;
GtkWidget *scrolled_treeview;
GtkWidget *treeview;
static GtkWidget *submenu_item1;
static GtkWidget *submenu_filelist_item2;
static GtkWidget *submenu_filelist_item3;
static GtkWidget *submenu_imglist_item3;
static GtkWidget *grid;
static GtkWidget *window;
static GtkWidget *list;
GtkWidget *wintitle;
static GtkWidget *text_view;
static GtkWidget *textbox_grid;
static GtkWidget *save_button;
static GtkWidget *rename_button;
static GtkWidget *delete_button;
static GtkWidget *pic_button;
static GtkWidget *scrolled_list;
static GtkWidget *scrolled_txt;
static GtkTextBuffer *buffer;
GtkListStore *store; //img store

static char current_folder[1024] = "";
static char current_file[1024] = ""; // Store Actual file name
char config_file_path[256];
char markup_buffer[256];

int selfromtreeview;
int selfromlistbox;
typedef struct
{
	GdkPixbuf *pixbuf;
	gchar *path;
}ImageInfo;

void null()
{}

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

void add_images_from_directory(GtkWidget *widget, gpointer user_data)
{
	gtk_list_store_clear(GTK_LIST_STORE(store));

	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char dir_path[1024];
	snprintf(dir_path, sizeof(dir_path), "%s%s%s/%s_files", home, notes_dir, current_folder, current_file);

	GFile *directory = g_file_new_for_path(dir_path);

	if (!g_file_query_exists(directory, NULL))
	{
		g_print("This note don't have pictures.\n");
		g_object_unref(directory);
		return;
	}

	GError *error = NULL;
	GFileEnumerator *enumerator = g_file_enumerate_children(directory, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NONE, NULL, &error);

	if (error != NULL)
	{
		g_print("Failed to enum files on directory: %s\n", error->message);
		g_error_free(error);
		g_object_unref(directory);
		return;
	}

	GFileInfo *info;
	while ((info = g_file_enumerator_next_file(enumerator, NULL, &error)) != NULL)
	{
		const char *filename = g_file_info_get_name(info);
		char *file_path = g_build_filename(dir_path, filename, NULL);

		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file_path, NULL);
			if (pixbuf != NULL)
		{
		int original_width = gdk_pixbuf_get_width(pixbuf);
		int target_width = 90;
		int target_height = gdk_pixbuf_get_height(pixbuf) * target_width / original_width;

		GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_width, target_height, GDK_INTERP_BILINEAR);

		GtkTreeIter iter;
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter, 0, scaled_pixbuf, -1);

		g_object_unref(pixbuf);
		g_object_unref(scaled_pixbuf);
		}
		g_free(file_path);
		g_object_unref(info);
	}
	g_file_enumerator_close(enumerator, NULL, NULL);
	g_object_unref(enumerator);
	g_object_unref(directory);
}


void add_image(GtkWidget *widget, gpointer user_data)
{
	GtkTreeIter iter;
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	dialog = gtk_file_chooser_dialog_new("Open File",
		GTK_WINDOW(window),
		action,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Open",
		GTK_RESPONSE_ACCEPT,
		NULL);

	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		filename = gtk_file_chooser_get_filename(chooser);
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
		if (pixbuf != NULL)
		{
			int original_width = gdk_pixbuf_get_width(pixbuf);
			int target_width = 100;
			int target_height = gdk_pixbuf_get_height(pixbuf) * target_width / original_width;

			const char *home = getenv("HOME");
			const char *notes_dir = "/.local/share/sgnotes/";
			char file_path[1024];
			snprintf(file_path, sizeof(file_path), "%s%s%s/%s_files", home, notes_dir, current_folder, current_file);

			g_mkdir_with_parents(file_path, 0755);

			// Always use .png extension for the copied files
			char unique_filename[1024];
			int counter = 0;
			do
			{
				snprintf(unique_filename, sizeof(unique_filename), "%s/%d.png", file_path, counter);
				counter++;
			} while (g_file_test(unique_filename, G_FILE_TEST_EXISTS));

			GFile *source_file = g_file_new_for_path(filename);
			GFile *destination_file = g_file_new_for_path(unique_filename);

			if (g_file_copy(source_file, destination_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL))
			{
				GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_width, target_height, GDK_INTERP_BILINEAR);
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter, 0, scaled_pixbuf, -1);
				g_object_unref(pixbuf);
				g_object_unref(scaled_pixbuf);
			}
			else
			{
				g_print("Error can't open output folder.\n");
			}
			g_object_unref(source_file);
			g_object_unref(destination_file);
		}
	}
	gtk_widget_destroy(dialog);
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
gboolean on_list_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		GtkWidget *submenu = GTK_WIDGET(data);
		return TRUE;
	}
	return FALSE;
}
// Entry dialogs behavior
static gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, GtkDialog *dialog) {
	if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
		gtk_dialog_response(dialog, GTK_RESPONSE_OK);
		return TRUE;
	}
	return FALSE;
}

static void on_entry_changed(GtkEditable *editable, gpointer user_data)
{
	GtkWidget *entry = GTK_WIDGET(editable);
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
	gchar *cleaned_text = g_strdelimit(g_strdup(text), "/\\\n", '\0');
	gtk_entry_set_text(GTK_ENTRY(entry), cleaned_text);
	g_free(cleaned_text);
}

void on_save_button_clicked(GtkButton *button, gpointer user_data)
{
	//Fix saving empty files
	if (current_file[0] == '\0')
	{
		return;
	}

	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char file_path[1024];
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home, notes_dir, current_folder, current_file);

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

void load_file_list()
{
	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char notes_path[1024];
	snprintf(notes_path, sizeof(notes_path), "%s%s%s", home, notes_dir, current_folder);

	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(notes_path)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_REG) // Disable showing directories
			{
				GtkWidget *label = gtk_label_new(entry->d_name);
				gtk_label_set_xalign(GTK_LABEL(label), 0.0);
				gtk_list_box_insert(GTK_LIST_BOX(list), label, -1);
				gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
				
			}
		}
		closedir(dir);
	}
}

int remove_recursive(const char *path)
{
	struct dirent *entry;
	DIR *dir = opendir(path);

	if (!dir)
	{
		return -1;
	}

	while ((entry = readdir(dir)))
	{
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
		{
			char full_path[PATH_MAX];
			snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
			if (entry->d_type == DT_DIR)
			{
				if (remove_recursive(full_path) != 0)
				{
					closedir(dir);
					return -1; 
				}
			}
			else 
			{
				if (remove(full_path) != 0)
				{
					closedir(dir);
					return -1; 
				}
			}
		}
	}

	closedir(dir);

	if (rmdir(path) != 0)
	{
		return -1;
	}

	return 0;
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
	char data_path[1024];
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home, notes_dir, current_folder, current_file);
	snprintf(data_path, sizeof(data_path), "%s%s%s/%s_files", home, notes_dir, current_folder, current_file);


	if (remove(file_path) == 0)
	{
		gtk_text_buffer_set_text(buffer, "", -1);
		current_file[0] = '\0';

		GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list));
		if (selected_row != NULL)
		{
			gtk_container_remove(GTK_CONTAINER(list), GTK_WIDGET(selected_row));
			gtk_widget_destroy(GTK_WIDGET(selected_row));
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
	if (remove_recursive(data_path) == 0)
	{
	printf("Directory removed: %s", data_path);
	}
	else
	{
		// Error handler
		/*GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Can't delete directory: '%s'", data_path);
		gtk_window_set_position(GTK_WINDOW(error_dialog), GTK_WIN_POS_CENTER);
		gtk_window_set_title(GTK_WINDOW(error_dialog), "Error deleting directory");
		gtk_dialog_run(GTK_DIALOG(error_dialog));
		gtk_widget_destroy(error_dialog);*/
		g_print("Error deleting data path, maybe this note don't have pictures");
	}
}


void on_delete_button_clicked(GtkButton *button, gpointer user_data)
{
	if (current_file[0] == '\0')
	{
		return;
	}

	dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure you want to delete this note: '%s'?", current_file);
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
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home, notes_dir, current_folder, filename);
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
	//gtk_widget_show(textbox_grid);
	gtk_widget_show(scrolled_txt);
	gtk_widget_show(pic_button);
	gtk_widget_show(scrolled_treeview);
	gtk_widget_show(treeview);
	gtk_widget_show(submenu_filelist_item2);
	gtk_widget_show(submenu_filelist_item3);
	gtk_widget_show(submenu_imglist_item3);
	gtk_widget_set_size_request(scrolled_list, 100, 150);
	gtk_widget_set_hexpand(scrolled_list, FALSE);
	load_file_content(filename);
	add_images_from_directory(GTK_WIDGET(treeview), user_data);
}



static void save_file(const gchar *filename)
{
	gchar *full_path;
	FILE *file;

	const char *home_dir = g_get_home_dir();

	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_folder, NULL);
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

void saveToFile(const gchar *text)
{
	gchar *full_path;
	FILE *file;

	const char *home_dir = g_get_home_dir();

	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_folder, NULL);
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
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
		gint result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		if (result == GTK_RESPONSE_NO || result == GTK_RESPONSE_CANCEL)
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
		GtkWidget *label = gtk_label_new(text);
		gtk_list_box_insert(GTK_LIST_BOX(list), label, -1);
						gtk_label_set_xalign(GTK_LABEL(label), 0.0);
				gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
		gtk_widget_show_all(GTK_WIDGET(list));
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
	gchar *imgoutput;
	gchar *imginput;
	FILE *file;

	dialog;
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
		g_signal_connect(G_OBJECT(entry), "key-press-event", G_CALLBACK(on_entry_key_press), dialog);
		gtk_widget_show_all(dialog);

	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	if (result == GTK_RESPONSE_OK)
	{
		text = gtk_entry_get_text(GTK_ENTRY(entry));
		const char *home_dir = g_get_home_dir();

		gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_folder, NULL);
		if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
			g_mkdir_with_parents(dir_path, 0700);
		output = g_build_filename(dir_path, text, NULL);

		// Check if the file already exists
		if (g_file_test(output, G_FILE_TEST_EXISTS))
		{
			dialog;
				dialog = gtk_message_dialog_new(NULL,
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
			"The file already exists. Do you want to overwrite it?");
			gtk_window_set_title(GTK_WINDOW(dialog), "Confirm");
			gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
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
		
		char img_path[255];
		strcpy(img_path, input);
		strcat(img_path, "_files");

		char img_path2[255];
		strcpy(img_path2, output);
		strcat(img_path2, "_files");
		
		imginput = g_build_filename(img_path, NULL);
		imgoutput = g_build_filename(img_path2, NULL);
		
		strcpy(current_file, text);
		if (rename(input, output) == 0)
		{
			printf("File moved successfully: %s > %s\n", input, output);
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
		if (rename(imginput, imgoutput) == 0)
		{
			printf("File moved successfully: %s > %s\n", imginput, imgoutput);
		}
		else
		{
			perror("Error moving file");
		}


	}
	else if (result == GTK_RESPONSE_CANCEL)
	{
		g_print("\n");
	}
	gtk_widget_destroy(dialog);
}

static void on_submenu_imglist_item1_selected()
{
const char *home = getenv("HOME");
		const char *notes_dir = "/.local/share/sgnotes/";
		char file_path[1024];
		file_path[0] = '\0';

		snprintf(file_path, sizeof(file_path), "%s%s%s/%s_files/%d.png", home, notes_dir, current_folder, current_file, selfromtreeview);

		char *command = g_strdup_printf("xdg-open \"%s\"", file_path);
		system(command);
		g_free(command);
}

int numeric_file_compare(const void *a, const void *b)
{
	const char *str1 = *(const char **)a;
	const char *str2 = *(const char **)b;

	int num1 = atoi(str1);
	int num2 = atoi(str2);

	return num1 - num2;
}

static void on_submenu_imglist_item2_selected(GtkWidget *widget, gpointer user_data)
{
	const char *home = getenv("HOME");
	const char *notes_dir = "/.local/share/sgnotes/";
	char file_path[1024];
	file_path[0] = '\0';

	snprintf(file_path, sizeof(file_path), "%s%s%s/%s_files/%d.png", home, notes_dir, current_folder, current_file, selfromtreeview);

	if (remove(file_path) == 0)
	{
		printf("deleted file:%s\n", file_path );
		DIR *dir;
		struct dirent *entry;
		char folder_path[1024];
		snprintf(folder_path, sizeof(folder_path), "%s%s%s/%s_files", home, notes_dir, current_folder, current_file);

		dir = opendir(folder_path);
		if (dir != NULL)
		{
			char **file_names = NULL;
			int file_count = 0;
			while ((entry = readdir(dir)) != NULL)
			{
				if (entry->d_type == DT_REG)
				{
					if (file_count == 0)
					{
						file_names = (char **)malloc(sizeof(char *));
					}
					else
					{
						file_names = (char **)realloc(file_names, (file_count + 1) * sizeof(char *));
					}
					file_names[file_count] = strdup(entry->d_name);
					file_count++;
				}
			}
			closedir(dir);
			qsort(file_names, file_count, sizeof(char *), numeric_file_compare);
			for (int i = 0; i < file_count; i++)
			{
				char old_name[1024];
				char new_name[1024];
				snprintf(old_name, sizeof(old_name), "%s/%s", folder_path, file_names[i]);
				snprintf(new_name, sizeof(new_name), "%s/%d.png", folder_path, i);
				rename(old_name, new_name);
				free(file_names[i]);
			}
			free(file_names);
		}
		gtk_list_store_clear(GTK_LIST_STORE(store));
		add_images_from_directory(GTK_WIDGET(treeview), user_data);
	}
	else
	{
		printf("Error deleting file:%s\n", file_path );
	}
}

static void on_submenu_item1_selected(GtkWidget *widget, gpointer data) {
	GtkWidget *dialog;
	GtkWidget *content_area;
	GtkWidget *entry;
	gint result;

	dialog = gtk_dialog_new_with_buttons("New Note:",
		GTK_WINDOW(data),
		GTK_DIALOG_MODAL,
		"OK",
		GTK_RESPONSE_OK,
		"Cancel",
		GTK_RESPONSE_CANCEL,
		NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(content_area), entry);
	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(on_entry_changed), dialog);
	g_signal_connect(G_OBJECT(entry), "key-press-event", G_CALLBACK(on_entry_key_press), dialog);

	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_widget_show_all(dialog);

	result = gtk_dialog_run(GTK_DIALOG(dialog));

	if (result == GTK_RESPONSE_OK) {
		const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
		saveToFile(text);
	} else if (result == GTK_RESPONSE_CANCEL) {
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
	if (config_file == NULL)
	{
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

gboolean on_treeview_clicked(GtkWidget *treeview, GdkEventButton *event, gpointer data)
{
	if (event->button == 3)
	{
		GtkTreePath *path = NULL;
		GtkTreeViewColumn *column = NULL;
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, &column, NULL, NULL))
		{
			gchar *path_string = gtk_tree_path_to_string(path);
			selfromtreeview = atoi(path_string);
			g_print("Picture selected: %d\n", selfromtreeview);
			GtkWidget *submenu = GTK_WIDGET(data);
			gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
			g_free(path_string);
			gtk_tree_path_free(path);
			return TRUE;
		}
	}
	else if (event->button == 1)
	{
			GtkTreePath *path = NULL;
		GtkTreeViewColumn *column = NULL;
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, &column, NULL, NULL))
		{
			gchar *path_string = gtk_tree_path_to_string(path);
			selfromtreeview = atoi(path_string);
			g_print("Picture selected: %d\n", selfromtreeview);
			on_submenu_imglist_item1_selected();
			g_free(path_string);
			gtk_tree_path_free(path);
			return TRUE;
		}
	}
	
	return TRUE;
}

gboolean on_listbox_clicked(GtkWidget *listbox, GdkEventButton *event, gpointer data)
{
	if (event->button == 3)
	{
		GtkListBoxRow *row = gtk_list_box_get_row_at_y(GTK_LIST_BOX(list), (gint)event->y);
		if (row != NULL)
		{
			selfromlistbox = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "index"));
			g_print("Picture selected: %d\n", selfromlistbox);
			GtkWidget *submenu = GTK_WIDGET(data);
			gtk_menu_popup_at_pointer(GTK_MENU(submenu), NULL);
			return TRUE;
		}
	}
	else if (event->button == 1)
	{
		GtkListBoxRow *row = gtk_list_box_get_row_at_y(GTK_LIST_BOX(list), (gint)event->y);
		if (row != NULL)
		{
			selfromlistbox = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(row), "index"));
			g_print("Picture selected: %d\n", selfromlistbox);
			on_submenu_imglist_item1_selected();
			return TRUE;
		}
	}
	return TRUE;
}

gboolean timeout_callback(gpointer user_data)
{
	if (current_file[0] == '\0')
	{
		//gtk_widget_hide(textbox_grid);
		gtk_widget_hide(scrolled_txt);
		gtk_widget_hide(pic_button);
		gtk_widget_hide(treeview);
		gtk_widget_hide(scrolled_treeview);
		gtk_widget_hide(submenu_filelist_item2);
		gtk_widget_hide(submenu_filelist_item3);
		gtk_widget_hide(submenu_imglist_item3);
		gtk_widget_set_hexpand(scrolled_list, TRUE);
	gtk_label_set_markup(GTK_LABEL(wintitle), "<b>Notes - SGNotes</b>");
	}
	else
	{
	snprintf(markup_buffer, sizeof(markup_buffer), "<b>%s - SGNotes</b>", current_file);
	gtk_label_set_markup(GTK_LABEL(wintitle), markup_buffer);
	}

	on_save_button_clicked(NULL, user_data);
	return G_SOURCE_CONTINUE;
}
