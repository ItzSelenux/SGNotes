#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

#ifdef WITHSOURCEVIEW
#include <gtksourceview/gtksource.h>
#endif

#include <sys/stat.h>

#define ML 256

GtkIconTheme *theme;
GtkIconInfo *info;
GdkPixbuf *icon;
GtkAccelGroup *accel_group;
GtkCellRenderer *filelist_renderer;
GtkListStore *imglist_store;
GtkTreeStore *filelist_store;
GtkTextMark *last_found = NULL;
static GtkTextBuffer *buffer;

gint fontsize=12, rfontsize=12, wordwrap=1,autosave=1,autosaverate=1,permitoverwrite=0,nautosaverate,
fix,showfind=0,nocsd=0,fcsd=0,selfromtreeview,selfromlistbox, nohome=0, initialized=0, usecsd=1,
resizablewidgets=0, saved=1, cooldown=0, visiblecfgmgr=0, timeout_id=0, fromfile=0;
gchar *pm, *home_dir, *notes_dir, *fontfamily, *fontstyle, *fontweight, *defworkspace, *program_icon;
GtkWidget *dialog,*scrolled_treeview,*treeview,*wintitle,*search_entry,*next_button,*prev_button;

GtkWidget *window, *mainbutton, *workspaces_dialog, *submenu_item_workspace, *submenu_item_newnote,
*submenu_filelist_item2, *sidebar_left, *sidebar_center, *textbox_container, *submenu_item_closefile, 
*submenu_filelist_item3,*submenu_imglist_item3,*grid,*window,*filelist,*text_view, *textbox_grid,
*submenu_item_save,*imgbox,*submenu_item_zoomreset, *submenu_item_zoomout, *submenu_item_zoomin,
*save_button,*rename_button,*delete_button,*pic_button,*scrolled_list,*scrolled_txt,
*paned_horiz1, *paned_horiz2,

*gwordwrap,*gfont, *gdefworkspace, *gpermitoverwrite, *gautosave, *gautosaverate, *gautosaverate_label, 
*gusecsd, *gresizablewidgets, *submenu_item_wordwrap;

gchar current_workspace[1024]="Default",current_file[1024]="";
const gchar* pver = mver;

GPtrArray *program_icon_names;
gchar *clicked_workspace = NULL;
gchar config_file_path[256],markup_buffer[256], notes_path[1024], workspaces_path[1024];

typedef struct
{
	GdkPixbuf *pixbuf;
	gchar *path;
}ImageInfo;

#ifndef WITHSOURCEVIEW
#define gtk_source_view_new gtk_text_view_new
#endif

#include "events.h"
#include "editor.h"
#include "fmanip.h"
#include "img.h"
#include "settings.h"
#include "cfgmgr.h"
#include "mainwindow.h"