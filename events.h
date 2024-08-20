void load_file_content(const char *filename);
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if ((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_f))
	{
		if (current_file[0] == '\0')
		{
		}
		else
		{
			if (showfind == 0)
			{
				gtk_widget_show(textbox_grid);
				gtk_widget_grab_focus(GTK_WIDGET(search_entry));
				showfind = 1;
			}
			else if (showfind == 1)
			{
				gtk_widget_hide(textbox_grid);
				gtk_widget_grab_focus(GTK_WIDGET(text_view));
				showfind = 0;
			}
		}
		return TRUE;
	}
	else if (event->keyval == GDK_KEY_Escape && showfind == 1)
	{
		gtk_widget_hide(textbox_grid);
		gtk_widget_grab_focus(GTK_WIDGET(text_view));
		showfind = 0;
	}
	else if (event->keyval == GDK_KEY_Escape && showfind == 0)
	{
		current_file[0] = '\0';
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
	return FALSE;
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
static gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, GtkDialog *dialog)
{
	if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter)
	{
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

void add_images_from_directory(GtkWidget *widget, gpointer user_data);
void on_list_item_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data)
{
	GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
	const char *filename = gtk_label_get_text(GTK_LABEL(label));
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

static void on_submenu_imglist_item1_selected();
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

