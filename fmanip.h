void on_submenu_item_workspace_selected();

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
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home, notes_dir, current_workspace, current_file);

	FILE *file = fopen(file_path, "w");
	if (file)
	{
		GtkTextIter start, end;
		gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
		gchar *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, FALSE);
		fprintf(file, "%s", text);
		fclose(file);
		g_free(text);
	}
}

void on_create_new_workspace(GtkButton *button, gpointer dialog)
{
	GtkWidget *entry, *dialog_new_workspace;
	GtkWidget *dialog_content;
	GtkWidget *label;

	dialog_new_workspace = gtk_dialog_new_with_buttons("New Workspace",
		GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
		GTK_DIALOG_MODAL,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Create", GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_window_set_position(GTK_WINDOW(dialog_new_workspace), GTK_WIN_POS_CENTER);
	dialog_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog_new_workspace))), dialog_content);

	label = gtk_label_new("Enter the name for the new workspace:");
	gtk_box_pack_start(GTK_BOX(dialog_content), label, FALSE, FALSE, 5);

	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(dialog_content), entry, FALSE, FALSE, 5);

	gtk_widget_show_all(dialog_new_workspace);

	gint result = gtk_dialog_run(GTK_DIALOG(dialog_new_workspace));

	if (result == GTK_RESPONSE_ACCEPT)
	{
		const gchar *workspace_name = gtk_entry_get_text(GTK_ENTRY(entry));

		snprintf(workspaces_path, sizeof(workspaces_path), "%s%s", home_dir, notes_dir);

		char new_workspace_path[1024];
		snprintf(new_workspace_path, sizeof(new_workspace_path), "%s%s", workspaces_path, workspace_name);

		if (access(new_workspace_path, F_OK) == 0)
		{
			GtkWidget *error_dialog = gtk_message_dialog_new
			(
				GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
				GTK_DIALOG_MODAL,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				"A workspace with the name '%s' already exists at '%s'.",
				workspace_name,
				new_workspace_path
			);
			gtk_window_set_position(GTK_WINDOW(error_dialog), GTK_WIN_POS_CENTER);

			gtk_dialog_run(GTK_DIALOG(error_dialog));
			gtk_widget_destroy(error_dialog);

			gtk_widget_destroy(dialog_new_workspace);

			on_create_new_workspace(button, NULL);
		}
		else
		{
			if (mkdir(new_workspace_path, 0700) == -1)
			{
				perror("Error creating directory");
			}
			else
			{
				printf("Workspace '%s' created at '%s'\n", workspace_name, new_workspace_path);
				strncpy(current_workspace, workspace_name, sizeof(current_workspace) - 1);
				gtk_widget_destroy(dialog_new_workspace);
				gtk_widget_destroy(workspaces_dialog);
				restart_program();
			}
		}
	}
	else
	{
		gtk_widget_destroy(dialog_new_workspace);
	}
}

void on_workspace_menu_item_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	if (clicked_workspace != NULL)
	{
		char objetive[1024];
		snprintf(objetive, sizeof(objetive), "%s%s%s", home_dir, notes_dir, clicked_workspace);
		printf("Workspace selected: %s\n", objetive);

		GtkWidget *dialog = gtk_message_dialog_new
		(
			GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(menuitem))),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_YES_NO,
			"Are you sure you want to remove '%s' ?",
			clicked_workspace
		);
gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

		gint result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		if (result == GTK_RESPONSE_YES)
		{
			if (rmdir(objetive) == 0)
			{
				printf("Directory %s removed successfully.\n", objetive);
				gtk_widget_destroy(workspaces_dialog);
				on_submenu_item_workspace_selected();
			}
			else
			{
				GtkWidget *error_dialog = gtk_message_dialog_new(
					GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(menuitem))),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,
					"Failed to remove directory '%s': %s",
					objetive, strerror(errno)
				);
				gtk_dialog_run(GTK_DIALOG(error_dialog));
				gtk_widget_destroy(error_dialog);
			}
		}
	}
}

void on_submenu_item_workspace_selected()
{
	DIR *dir;
	struct dirent *entry;

	GtkWidget *dialog, *content_area, *tree_view, *scrolled_window;
	GtkListStore *list_store;
	GtkTreeIter iter;

	workspaces_dialog = gtk_dialog_new_with_buttons("Select Workspace",
		NULL,
		GTK_DIALOG_MODAL,
		"_Cancel",
		GTK_RESPONSE_CANCEL,
		"_Create New Workspace",
		GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_window_set_position(GTK_WINDOW(workspaces_dialog), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request(workspaces_dialog, 333, 333);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(workspaces_dialog));

	list_store = gtk_list_store_new(1, G_TYPE_STRING);

	if ((dir = opendir(workspaces_path)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				gtk_list_store_append(list_store, &iter);
				gtk_list_store_set(list_store, &iter, 0, entry->d_name, -1);
			}
		}
		closedir(dir);
	}

	tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
	g_object_unref(list_store);

	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Workspaces", gtk_cell_renderer_text_new(), "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_workspace_row_activated), workspaces_dialog);

	g_signal_connect(tree_view, "button-press-event", G_CALLBACK(on_workspace_button_press), NULL);

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
	gtk_widget_set_vexpand(scrolled_window, TRUE);
	gtk_widget_set_hexpand(scrolled_window, TRUE);
	gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);

	gtk_widget_show_all(workspaces_dialog);

	if (gtk_dialog_run(GTK_DIALOG(workspaces_dialog)) == GTK_RESPONSE_ACCEPT)
	{
		on_create_new_workspace(NULL, workspaces_dialog);
	}

	gtk_widget_destroy(workspaces_dialog);
}

void load_file_list()
{
	snprintf(notes_path, sizeof(notes_path), "%s%s%s", home_dir, notes_dir, current_workspace);

	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(notes_path)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_REG)
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
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home, notes_dir, current_workspace, current_file);
	snprintf(data_path, sizeof(data_path), "%s%s%s/%s_files", home, notes_dir, current_workspace, current_file);


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
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(dialog), "WARNING");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	if (response == GTK_RESPONSE_YES)
	{
		delete_current_file();
	}
}


static void save_file(const gchar *filename)
{
	gchar *full_path;
	FILE *file;

	const char *home_dir = g_get_home_dir();

	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_workspace, NULL);
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

	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_workspace, NULL);
	if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
		g_mkdir_with_parents(dir_path, 0700);

	full_path = g_build_filename(dir_path, text, NULL);

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

		gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_workspace, NULL);
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
static void on_submenu_item1_selected(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog, *content_area, *entry;
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