void readconf()
{
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
		if (file = NULL)
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
	wraptext = (wraptext == 0) ? 1 : 0;
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), (wraptext == 0) ? GTK_WRAP_NONE : GTK_WRAP_WORD_CHAR);

	FILE *config_file = fopen(config_file_path, "w");
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