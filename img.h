void add_images_from_directory(GtkWidget *widget, gpointer user_data)
{
	gtk_list_store_clear(GTK_LIST_STORE(imglist_store));

	char dir_path[1024];
	snprintf(dir_path, sizeof(dir_path), "%s%s%s/%s_files", home_dir, notes_dir, current_workspace, current_file);

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
		gtk_list_store_append(imglist_store, &iter);
		gtk_list_store_set(imglist_store, &iter, 0, scaled_pixbuf, -1);

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

			char file_path[1024];
			snprintf(file_path, sizeof(file_path), "%s%s%s/%s_files", home_dir, notes_dir, current_workspace, current_file);

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
				gtk_list_store_append(imglist_store, &iter);
				gtk_list_store_set(imglist_store, &iter, 0, scaled_pixbuf, -1);
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


static void on_submenu_imglist_item1_selected()
{
		char file_path[1024];
		file_path[0] = '\0';

		snprintf(file_path, sizeof(file_path), "%s%s%s/%s_files/%d.png", home_dir, notes_dir, current_workspace, current_file, selfromtreeview);

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

	char file_path[1024];
	file_path[0] = '\0';

	snprintf(file_path, sizeof(file_path), "%s%s%s/%s_files/%d.png", home_dir, notes_dir, current_workspace, current_file, selfromtreeview);

	if (remove(file_path) == 0)
	{
		g_print("deleted file:%s\n", file_path );
		DIR *dir;
		struct dirent *entry;
		char folder_path[1024];
		snprintf(folder_path, sizeof(folder_path), "%s%s%s/%s_files", home_dir, notes_dir, current_workspace, current_file);

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
		gtk_list_store_clear(GTK_LIST_STORE(imglist_store));
		add_images_from_directory(GTK_WIDGET(treeview), user_data);
	}
	else
	{
		g_print("Error deleting file:%s\n", file_path );
	}
}