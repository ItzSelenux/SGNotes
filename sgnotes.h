#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <sys/stat.h>

#define ML 256

GtkIconTheme *theme;
GtkIconInfo *info;
GdkPixbuf *icon;
GtkAccelGroup *accel_group;
GtkListStore *store; //img store
GtkTextMark *last_found = NULL;
static GtkTextBuffer *buffer;

int wraptext,fix,showfind=0,nocsd = 0,selfromtreeview,selfromlistbox;
char *pm, *home_dir;
GtkWidget *dialog,*scrolled_treeview,*treeview,*wintitle,*search_entry,*next_button,*prev_button;

static GtkWidget *submenu_item1, *submenu_filelist_item2,*submenu_filelist_item3,*submenu_imglist_item3,
*grid,*window,*list,*text_view, *textbox_grid,*save_button,*rename_button,*delete_button,*pic_button,
*scrolled_list,*scrolled_txt;

static char current_folder[1024]="Default",current_file[1024]="";
const char* pver = mver;
char config_file_path[256],markup_buffer[256];

typedef struct
{
	GdkPixbuf *pixbuf;
	gchar *path;
}ImageInfo;

#include "events.h"
#include "editor.h"
#include "fmanip.h"
#include "img.h"
#include "settings.h"
#include "mainwindow.h"