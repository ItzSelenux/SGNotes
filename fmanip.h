void on_submenu_item_workspace_selected(void);
gboolean timeout_callback(gpointer user_data);

void on_save_button_clicked(GtkButton *button, gpointer user_data)
{
	//Fix saving empty files
	if (current_file[0] == '\0')
	{
		return;
	}

	gchar file_path[8192];
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home_dir, notes_dir, current_workspace, current_file);

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

		gchar new_workspace_path[8192];
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
				g_print("Workspace '%s' created at '%s'\n", workspace_name, new_workspace_path);
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
		gchar objetive[1024];
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
				g_print("Directory %s removed successfully.\n", objetive);
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

void on_submenu_item_workspace_selected(void)
{
	DIR *dir;
	struct dirent *entry;

	GtkWidget *content_area, *tree_view, *scrolled_window;
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

void load_file_list(void)
{
	GtkTreeViewColumn *column;
	//
	GtkTreeIter iter;
	char notes_path[8192];

	snprintf(notes_path, sizeof(notes_path), "%s%s%s", home_dir, notes_dir, current_workspace);

	DIR *dir;
	struct dirent *entry;

	filelist_store = gtk_tree_store_new(1, G_TYPE_STRING);

	if ((dir = opendir(notes_path)) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_REG)
			{
				gtk_tree_store_append(filelist_store, &iter, NULL);
				gtk_tree_store_set(filelist_store, &iter, 0, entry->d_name, -1);
			}
		}
		closedir(dir);
	}

	if (GTK_IS_TREE_VIEW(filelist))
	{
		gtk_tree_view_set_model(GTK_TREE_VIEW(filelist), GTK_TREE_MODEL(filelist_store));
		column = gtk_tree_view_column_new_with_attributes("File Name", filelist_renderer, "text", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(filelist), column);
	}
	g_object_unref(imglist_store);
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

void delete_current_file(void)
{
	if (current_file[0] == '\0')
	{
		return;
	}

	char file_path[8192];
	char data_path[8192];
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home_dir, notes_dir, current_workspace, current_file);
	snprintf(data_path, sizeof(data_path), "%s%s%s/%s_files", home_dir, notes_dir, current_workspace, current_file);

	if (remove(file_path) == 0)
	{
		gtk_text_buffer_set_text(buffer, "", -1);
		current_file[0] = '\0';

		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(filelist));
		GtkTreeIter iter;

		if (gtk_tree_selection_get_selected(selection, (GtkTreeModel **)&filelist_store, &iter))
		{
			gtk_tree_store_remove(filelist_store, &iter);
			gtk_widget_hide(textbox_grid);
		}
	}
	else
	{
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window), 
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, 
			GTK_MESSAGE_ERROR, 
			GTK_BUTTONS_OK, 
			"Can't delete file: '%s'", 
			current_file);

		gtk_window_set_position(GTK_WINDOW(error_dialog), GTK_WIN_POS_CENTER);
		gtk_window_set_title(GTK_WINDOW(error_dialog), "Error deleting file");
		gtk_dialog_run(GTK_DIALOG(error_dialog));
		gtk_widget_destroy(error_dialog);
	}

	if (remove_recursive(data_path) == 0)
	{
		g_print("Directory removed: %s\n", data_path);
	}
	else
	{
		g_print("Error deleting data path, maybe this note doesn't have pictures\n");
	}
}

void on_delete_button_clicked(GtkButton *button, gpointer user_data)
{
	if (!saved)
	{
		gint opt = show_file_warning();
		if (opt == 1)
		{
			timeout_callback(NULL);
		}
		else if (opt == 2)
		{
			g_warning("File was closed without saving modifications, and all changes were lost.");
			togglesave(text_view, GINT_TO_POINTER(1));
		}
		else
		{
			return;
		}
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(filelist));
	GtkTreeIter iter;

	if (gtk_tree_selection_get_selected(selection, (GtkTreeModel **)&filelist_store, &iter))
	{
		gchar *selected_value;
		gtk_tree_model_get(GTK_TREE_MODEL(filelist_store), &iter, 0, &selected_value, -1);
			GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(filelist_store), &iter);
			gtk_tree_view_row_activated(GTK_TREE_VIEW(filelist), path, gtk_tree_view_get_column(GTK_TREE_VIEW(filelist), 0));
			gtk_tree_path_free(path);
		g_strlcpy(current_file, selected_value, sizeof(current_file));

		dialog = gtk_message_dialog_new(GTK_WINDOW(window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
			"Are you sure you want to delete '%s'?", selected_value);

		gtk_window_set_title(GTK_WINDOW(dialog), "WARNING");
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

		gint response = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);

		if (response == GTK_RESPONSE_YES)
		{
			delete_current_file();
			saved = 1;
			gtk_widget_activate(submenu_item_closefile);
		}
		g_free(selected_value);
	}
}

void on_export_button_clicked(GtkButton *button, gpointer user_data)
{
	if (current_file[0] == '\0')
	{
		return;
	}

	gchar file_path[8192];
	gchar data_path[8192];

	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home_dir, notes_dir, current_workspace, current_file);
	snprintf(data_path, sizeof(data_path), "%s%s%s/%s_files", home_dir, notes_dir, current_workspace, current_file);

	GtkWidget *file_chooser_dialog;
	file_chooser_dialog = gtk_file_chooser_dialog_new("Select Destination",
		GTK_WINDOW(window),
		GTK_FILE_CHOOSER_ACTION_SAVE,
		"_Cancel", GTK_RESPONSE_CANCEL,
		"_Save", GTK_RESPONSE_ACCEPT,
		NULL);

	if (gtk_dialog_run(GTK_DIALOG(file_chooser_dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(file_chooser_dialog);
		gchar *destination_file = gtk_file_chooser_get_filename(chooser);
		GError *error = NULL;

		if (g_file_test(file_path, G_FILE_TEST_EXISTS))
		{
			if (g_file_copy(g_file_new_for_path(file_path),
				g_file_new_for_path(destination_file),
				G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error))
			{
				GtkWidget *success_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_OK,
					"File copied successfully to '%s'.", destination_file);
				gtk_dialog_run(GTK_DIALOG(success_dialog));
				gtk_widget_destroy(success_dialog);
			}
			else
			{
				GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,
					"Failed to copy file: %s", error->message);
				gtk_dialog_run(GTK_DIALOG(error_dialog));
				gtk_widget_destroy(error_dialog);
				g_error_free(error);
			}
		}
		else
		{
			GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
				GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				"Source file '%s' does not exist.", file_path);
			gtk_dialog_run(GTK_DIALOG(error_dialog));
			gtk_widget_destroy(error_dialog);
		}
		g_free(destination_file);
	}
	gtk_widget_destroy(file_chooser_dialog);
}

void save_file(const gchar *filename)
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
	gint note_overwrite = 0;

	const char *home_dir = g_get_home_dir();
	gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_workspace, NULL);

	if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
	{
		g_mkdir_with_parents(dir_path, 0700);
	}

	full_path = g_build_filename(dir_path, text, NULL);

	if (g_file_test(full_path, G_FILE_TEST_EXISTS))
	{
		GtkWidget *dialog;

		if (permitoverwrite)
		{
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
				return;
			}
			else
			{
				note_overwrite = 1;
			}
		}
		else
		{
			show_error_dialog("File already exists and overwriting is not allowed.");
			g_free(full_path);
			g_free(dir_path);
			return;
		}
	}

	file = fopen(full_path, "w");
	if (file)
	{
		fclose(file);
		g_print("Note saved to %s\n", full_path);

		if (!note_overwrite)
		{
			GtkTreeIter iter;
			gtk_tree_store_append(filelist_store, &iter, NULL);
			gtk_tree_store_set(filelist_store, &iter, 0, text, -1);

			GtkTreeIter first_iter;
			if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(filelist_store), &first_iter))
			{
				gtk_tree_store_move_before(filelist_store, &iter, &first_iter);
			}
		}
	}
	else
	{
		g_print("Unable to create the file.\n");
		GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"Unable to create the file: \n '%s'", text);
		gtk_window_set_title(GTK_WINDOW(error_dialog), "Unable to create the file.");
		gtk_dialog_run(GTK_DIALOG(error_dialog));
		gtk_widget_destroy(error_dialog);
	}
	g_free(full_path);
	g_free(dir_path);
}


void on_rename_button_clicked(GtkButton *button, gpointer user_data)
{
	if (!saved)
	{
		gint opt = show_file_warning();
		if (opt == 1)
		{
			timeout_callback(NULL);
		}
		else if (opt == 2)
		{
			g_warning("File was closed without saving modifications, and all changes were lost.");
			togglesave(text_view, GINT_TO_POINTER(1));
		}
		else
		{
			return;
		}
	}

	gint note_overwrite = 0;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(filelist));
	GtkTreeModel *model;
	GtkTreeIter iter;

	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		g_warning("No file selected.");
		return;
	}

	g_autofree gchar *selected_value = NULL;
	gtk_tree_model_get(model, &iter, 0, &selected_value, -1);

	GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_view_row_activated(GTK_TREE_VIEW(filelist), path, gtk_tree_view_get_column(GTK_TREE_VIEW(filelist), 0));
	gtk_tree_path_free(path);

	const gchar *text = "";
	g_autofree gchar *output = NULL;
	g_autofree gchar *input = NULL;
	g_autofree gchar *imgoutput = NULL;
	g_autofree gchar *imginput = NULL;

	GtkWidget *rename_dialog = gtk_dialog_new_with_buttons("Rename Note:",
		NULL,
		GTK_DIALOG_MODAL,
		"OK", GTK_RESPONSE_OK,
		"Cancel", GTK_RESPONSE_CANCEL,
		NULL);
	gtk_window_set_position(GTK_WINDOW(rename_dialog), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request(rename_dialog, 333, -1);

	GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(rename_dialog));
	GtkWidget *entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(content_area), entry);

	g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(on_entry_changed), rename_dialog);
	g_signal_connect(G_OBJECT(entry), "key-press-event", G_CALLBACK(on_entry_key_press), rename_dialog);
	gtk_widget_show_all(rename_dialog);

	gint result = gtk_dialog_run(GTK_DIALOG(rename_dialog));
	if (result == GTK_RESPONSE_OK)
	{
		text = gtk_entry_get_text(GTK_ENTRY(entry));
		if (g_strcmp0(text, "") == 0)
		{
			g_warning("File name cannot be empty.");
			gtk_widget_destroy(rename_dialog);
			return;
		}

		const char *home_dir = g_get_home_dir();
		g_autofree gchar *dir_path = g_build_filename(home_dir, ".local", "share", "sgnotes", current_workspace, NULL);

		if (!g_file_test(dir_path, G_FILE_TEST_EXISTS))
		{
			g_mkdir_with_parents(dir_path, 0700);
		}

		output = g_build_filename(dir_path, text, NULL);

		if (g_file_test(output, G_FILE_TEST_EXISTS))
		{
			if (permitoverwrite)
			{
				GtkWidget *overwrite_dialog = gtk_message_dialog_new(NULL,
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					"The file already exists. Do you want to overwrite it?");
				gtk_window_set_title(GTK_WINDOW(overwrite_dialog), "Confirm");
				gtk_window_set_position(GTK_WINDOW(overwrite_dialog), GTK_WIN_POS_CENTER);
				gint overwrite_result = gtk_dialog_run(GTK_DIALOG(overwrite_dialog));
				gtk_widget_destroy(overwrite_dialog);

				if (overwrite_result != GTK_RESPONSE_YES)
				{
					gtk_widget_destroy(rename_dialog);
					return;
				}
				else
				{
					note_overwrite = 1;
				}
			}
			else
			{
				show_error_dialog("File already exist");
				gtk_widget_destroy(rename_dialog);
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

		imginput = g_strdup(img_path);
		imgoutput = g_strdup(img_path2);

		if (rename(input, output) == 0)
		{
			g_print("File moved successfully: %s > %s\n", input, output);
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, text, -1);
		}
		else
		{
			g_warning("Error moving file: %s", g_strerror(errno));
		}

		if (g_file_test(imginput, G_FILE_TEST_EXISTS))
		{
			if (rename(imginput, imgoutput) == 0)
			{
				g_print("Image folder moved successfully: %s > %s\n", imginput, imgoutput);
			}
			else
			{
				g_warning("Error moving image folder: %s", g_strerror(errno));
			}
		}
		else
		{
			g_print("Image folder does not exist, skipping: %s\n", imginput);
		}

		if (note_overwrite == 1)
		{
			current_file[0] = '\0';
			gtk_widget_hide(scrolled_txt);
			gtk_widget_hide(pic_button);
			gtk_widget_hide(treeview);
			gtk_widget_hide(scrolled_treeview);
			gtk_widget_hide(submenu_filelist_item2);
			gtk_widget_hide(submenu_filelist_item3);
			gtk_widget_hide(submenu_imglist_item3);
			gtk_widget_hide(textbox_grid);
			gtk_widget_set_hexpand(scrolled_list, TRUE);
			gtk_label_set_markup(GTK_LABEL(wintitle), "<b>Notes - SGNotes</b>");
			gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
			g_print("File entry removed from tree.\n");
		}
		else
		{
			strcpy(current_file, text);
		}
	}
	gtk_widget_destroy(rename_dialog);
}

static void submenu_item_newnote_selected(GtkWidget *widget, gpointer data)
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
