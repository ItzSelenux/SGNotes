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
GtkListStore *store; //img store
GtkTextMark *last_found = NULL;
static GtkTextBuffer *buffer;

int wraptext,fix,showfind,selfromtreeview,selfromlistbox;
char *pm;
GtkWidget *dialog,*scrolled_treeview,*treeview,*wintitle,*search_entry,*next_button,*prev_button;

static GtkWidget *submenu_item1, *submenu_filelist_item2,*submenu_filelist_item3,*submenu_imglist_item3,
*grid,*window,*list,*text_view, *textbox_grid,*save_button,*rename_button,*delete_button,*pic_button,
*scrolled_list,*scrolled_txt;

static char current_folder[1024]="",current_file[1024]="";
const char* pver = mver;
char config_file_path[256],markup_buffer[256];

typedef struct
{
	GdkPixbuf *pixbuf;
	gchar *path;
}ImageInfo;

void load_file_content(const char *filename);
void saveToFile(const gchar *text);
void on_save_button_clicked(GtkButton *button, gpointer user_data);
static void on_submenu_imglist_item1_selected();
void add_images_from_directory(GtkWidget *widget, gpointer user_data);

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
	while (fgets(line, sizeof(line), file) != NULL)
	{
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

static void on_submenu_item2_selected(GtkWidget *widget, gpointer data)
{
	const char *home_dir = getenv("HOME");
	char config_file_path[256];
	if (home_dir == NULL)
	{
		fprintf(stderr, "Error: HOME environment variable is not set.\n");
		exit(1);
	}

	snprintf(config_file_path, sizeof(config_file_path), "%s/.config/sgnotes.conf", home_dir);

	FILE *config_file = fopen(config_file_path, "r");
	if (config_file == NULL)
	{
		wraptext = 0;
	}
	else
	{
		char line[256];
		while (fgets(line, sizeof(line), config_file))
		{
			if (strstr(line, "wraptext=") == line)
			{
				int value;
				if (sscanf(line, "wraptext=%d", &value) == 1)
				{
					wraptext = value;
				}
				break;
			}
		}
		fclose(config_file);
	}

	wraptext = (wraptext == 0) ? 1 : 0;
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), (wraptext == 0) ? GTK_WRAP_NONE : GTK_WRAP_WORD_CHAR);

	config_file = fopen(config_file_path, "w");
	if (config_file != NULL)
	{
		fprintf(config_file, "wraptext=%d\n", wraptext);
		fclose(config_file);
	}
	else
	{
		fprintf(stderr, "Error: Failed to open file.\n");
	}
}

void on_submenu_item3_selected(GtkMenuItem *menuitem, gpointer userdata)
{
	dialog = gtk_about_dialog_new();

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SGNotes");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), pver);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Copyright © 2024 ItzSelenux for Simple GTK Desktop Environment");
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
