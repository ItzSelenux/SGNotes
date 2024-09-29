void readconf()
{
	if (home_dir == NULL)
	{
		g_error("HOME environment variable is not set.");
		nohome = 1;
		return;
	}
	else
	{
		snprintf(config_file_path, sizeof(config_file_path), "%s/.config/sgnotes.conf", home_dir);

		FILE *file = fopen(config_file_path, "r");

		if (file == NULL)
		{
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
				else if (strcmp(name, "fontsize") == 0)
				{
					fontsize = atoi(value_str);
					fontsize = (fontsize > 100 || fontsize < 1) ? 12 : fontsize;
				}
			}
		}
		fclose(file);
	}
	g_print("WrapText: %d\nFix: %d\nFontSize: %d\n", wraptext, fix, fontsize);
}

static void on_submenu_item2_selected(GtkWidget *widget, gpointer data)
{
	wraptext = (wraptext == 0) ? 1 : 0;
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), (wraptext == 0) ? GTK_WRAP_NONE : GTK_WRAP_WORD_CHAR);

	if (!nohome)
	{
		FILE *config_file = fopen(config_file_path, "w");
		if (config_file != NULL)
		{
			fprintf(config_file, "wraptext=%d\n", wraptext);
			fprintf(config_file, "fontsize=%d\n", fontsize);
			fclose(config_file);
		}
		else
		{
			g_warning("Failed to open file.");
		}
	}
}