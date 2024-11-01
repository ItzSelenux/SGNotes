void updateuistyle(void)
{
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), (wordwrap == 0) ? GTK_WRAP_NONE : GTK_WRAP_WORD_CHAR);
		GtkCssProvider *provider = gtk_css_provider_new();
		gchar *css = g_strdup_printf("textview { font-family: \"%s\"; font-size: %dpt; font-style: %s; font-weight: %s; }", 
			fontfamily, fontsize, fontstyle, fontweight);
		gtk_css_provider_load_from_data(provider, css, -1, NULL);
		g_free(css);
		gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
		GTK_STYLE_PROVIDER(provider),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		g_object_unref(provider);
}

void readconf(void)
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
				if (strcmp(name, "wordwrap") == 0)
				{
					wordwrap = atoi(value_str);
				}
				else if (strcmp(name, "fontfamily") == 0)
				{
					g_strchomp(value_str);
					fontfamily = strdup(value_str);
				}
				else if (strcmp(name, "fontsize") == 0)
				{
					fontsize = atoi(value_str);
					rfontsize = atoi(value_str);
				}
				else if (strcmp(name, "fontstyle") == 0)
				{
					g_strchomp(value_str);
					fontstyle = strdup(value_str);
				}
				else if (strcmp(name, "fontweight") == 0)
				{
					g_strchomp(value_str);
					fontweight = strdup(value_str);
				}
				else if (strcmp(name, "defworkspace") == 0)
				{
					if (!initialized)
					{
						initialized = 1;
						g_strchomp(value_str);
						defworkspace = strdup(value_str);
						strncpy(current_workspace, strdup(value_str), sizeof(current_workspace) - 1);
					}
				}
				else if (strcmp(name, "permitoverwrite") == 0)
				{
					permitoverwrite = atoi(value_str);
				}
				else if (strcmp(name, "autosave") == 0)
				{
					autosave = atoi(value_str);
				}
				else if (strcmp(name, "autosaverate") == 0)
				{
					autosaverate = atoi(value_str);
				}
				else if (strcmp(name, "usecsd") == 0)
				{
					usecsd = atoi(value_str);
					if (!fcsd)
					{
						nocsd = (usecsd == 0);
					}
				}
				else if (strcmp(name, "resizablewidgets") == 0)
				{
					resizablewidgets = atoi(value_str);
				}
			}
		}
		fclose(file);
	}
	g_print("wordwrap: %d\nfont: %s\nfontsize: %d\ndefworkspace: %s\npermitoverwrite: %d\nautosave: %d\nautosaverate: %d\nusecsd: %d\nresizablewidgets: %d\n",
		wordwrap, fontfamily, fontsize, defworkspace, permitoverwrite, autosave, autosaverate, usecsd, resizablewidgets);
}

void updateconf(GtkWidget *widget, gpointer data, gint isdefault)
{
	if (!isdefault && visiblecfgmgr)
	{
		wordwrap = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gwordwrap));

		fontfamily = g_strdup(pango_font_family_get_name(gtk_font_chooser_get_font_family(GTK_FONT_CHOOSER(gfont))));

		PangoFontDescription *font_desc = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(gfont));

		PangoStyle font_style = pango_font_description_get_style(font_desc);
		switch (font_style)
		{
			case PANGO_STYLE_ITALIC:
				fontstyle = "Italic";
				break;
			case PANGO_STYLE_OBLIQUE:
				fontstyle = "Oblique";
				break;
			case PANGO_STYLE_NORMAL:
			default:
				fontstyle = "Normal";
				break;
		}
		PangoWeight font_weight = pango_font_description_get_weight(font_desc);
		switch (font_weight)
		{
			case PANGO_WEIGHT_THIN:
				fontweight = "100";
				break;
			case PANGO_WEIGHT_ULTRALIGHT:
			case PANGO_WEIGHT_LIGHT:
				fontweight = "200";
				break;
			case PANGO_WEIGHT_SEMILIGHT:
				fontweight = "300";
				break;
			case PANGO_WEIGHT_BOOK:
			case PANGO_WEIGHT_NORMAL:
				fontweight = "400";
				break;
			case PANGO_WEIGHT_MEDIUM:
				fontweight = "500";
				break;
			case PANGO_WEIGHT_SEMIBOLD:
				fontweight = "600";
				break;
			case PANGO_WEIGHT_BOLD:
				fontweight = "700";
				break;
			case PANGO_WEIGHT_ULTRABOLD:
			case PANGO_WEIGHT_HEAVY:
				fontweight = "800";
				break;
			case PANGO_WEIGHT_ULTRAHEAVY:
				fontweight = "900";
				break;
			default:
				fontweight = "400";
				break;
		}

		int font_size = pango_font_description_get_size(font_desc);
		if (!pango_font_description_get_size_is_absolute(font_desc))
		{
			font_size = font_size / PANGO_SCALE;
		}
		rfontsize = font_size;
		fontsize = font_size;
		defworkspace = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gdefworkspace));
		permitoverwrite = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gpermitoverwrite));
		autosave = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gautosave));
		autosaverate = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gautosaverate));
		usecsd = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gusecsd));
		resizablewidgets = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gresizablewidgets));
	}
	else if (!visiblecfgmgr)
	{
		wordwrap = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(submenu_item_wordwrap));
	}
	else
	{
		wordwrap = 1;
		fontfamily = "";
		fontsize = 10;
		fontstyle = "Regular";
		fontweight = "400";
		defworkspace = "Default";
		permitoverwrite = 0;
		autosave = 1;
		autosaverate = 0;
		usecsd = 1;
		resizablewidgets = 1;
	}

	if (!nohome)
	{
		FILE *config_file = fopen(config_file_path, "w");
		if (config_file != NULL)
		{
			fprintf(config_file, "[SGNotes configuration file]\n");
			fprintf(config_file, "[View]\n");
			fprintf(config_file, "wordwrap=%d\n", wordwrap);
			fprintf(config_file, "fontfamily=%s\n", fontfamily);
			fprintf(config_file, "fontsize=%d\n", fontsize);
			fprintf(config_file, "fontstyle=%s\n", fontstyle);
			fprintf(config_file, "fontweight=%s\n", fontweight);
			fprintf(config_file, "[File]\n");
			fprintf(config_file, "defworkspace=%s\n", defworkspace);
			fprintf(config_file, "permitoverwrite=%d\n", permitoverwrite);
			fprintf(config_file, "autosave=%d\n", autosave);
			fprintf(config_file, "autosaverate=%d\n", autosaverate);
			fprintf(config_file, "[Window]\n");
			fprintf(config_file, "usecsd=%d\n", usecsd);
			fprintf(config_file, "resizablewidgets=%d\n", resizablewidgets);
			fclose(config_file);
		}
		else
		{
			g_warning("Failed to open file.");
		}
	}
	updateuistyle();
	if (visiblecfgmgr)
	{
		if (!autosave)
		{
			if (timeout_id != 0)
			{
				g_source_remove(timeout_id);
				timeout_id = 0;
			}
		}
		else
		{
			if (timeout_id != 0)
			{
				g_source_remove(timeout_id);
			}

			if (autosaverate == 0)
			{
				timeout_id = g_timeout_add(100, timeout_callback, NULL);
			}
			else
			{
				timeout_id = g_timeout_add_seconds(autosaverate, timeout_callback, NULL);
			}
		}
		gtk_widget_destroy(dialog);
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item_wordwrap), wordwrap);
	visiblecfgmgr = 0;
	return;
}
