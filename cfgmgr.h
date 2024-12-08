void toggleautosave(void)
{
	if (gtk_widget_get_visible(gautosaverate)) 
	{
		gtk_widget_hide(gautosaverate);
		gtk_widget_hide(gautosaverate_label);
	}
	else
	{
		gtk_widget_show(gautosaverate);
		gtk_widget_show(gautosaverate_label);
	}
}

void closecfg(void)
{
	visiblecfgmgr = 0;
	gtk_widget_destroy(dialog);
}

void showcfg(void)
{
	if (visiblecfgmgr == 1)
	{
		return;
	}
	visiblecfgmgr = 1;
	dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog), "Settings - SGNotes");
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
	gtk_widget_set_size_request(dialog, 400, 200);
	window_set_icon(GTK_WINDOW(window), program_icon);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	g_signal_connect(dialog, "destroy", G_CALLBACK(closecfg), dialog);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

	GtkWidget *defbtn, *applybtn;

	GtkWidget *notebook = gtk_notebook_new();
		GtkWidget *viewgrid = gtk_grid_new();
			gtk_grid_set_row_spacing(GTK_GRID(viewgrid), 5);
			gtk_grid_set_column_homogeneous(GTK_GRID(viewgrid), TRUE);

			gwordwrap = gtk_check_button_new();
			gfont = gtk_font_button_new();
				gtk_font_button_set_show_style(GTK_FONT_BUTTON(gfont), FALSE);

		GtkWidget *filegrid = gtk_grid_new();
			gtk_grid_set_row_spacing(GTK_GRID(filegrid), 5);
			gtk_grid_set_column_homogeneous(GTK_GRID(filegrid), TRUE);

			gdefworkspace = gtk_combo_box_text_new();
				struct dirent *entry;
				DIR *dir;
				if ((dir = opendir(workspaces_path)) != NULL)
				{
					while ((entry = readdir(dir)) != NULL)
					{
						if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
						{
							gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gdefworkspace), NULL, entry->d_name);
						}
					}
					closedir(dir);
				}
				gtk_combo_box_set_active(GTK_COMBO_BOX(gdefworkspace), 0);
			gpermitoverwrite = gtk_check_button_new();
			gautosave = gtk_check_button_new();
			GtkAdjustment *gautosaverate_adjustment = gtk_adjustment_new(1, 0, 60, 1, 1, 0); 
				gautosaverate = gtk_spin_button_new(gautosaverate_adjustment, 1, 0);

		GtkWidget *windowgrid = gtk_grid_new();
			gtk_grid_set_row_spacing(GTK_GRID(windowgrid), 10);
			gtk_grid_set_column_homogeneous(GTK_GRID(windowgrid), TRUE);

			gusecsd = gtk_check_button_new();
			gresizablewidgets = gtk_check_button_new();

			GtkWidget *applybox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
				defbtn = gtk_button_new_with_label("Default");
				applybtn  = gtk_button_new_with_label("Apply");

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), viewgrid, gtk_label_new("View"));
		gtk_grid_attach(GTK_GRID(viewgrid), gtk_label_new("Word Wrap"), 0, 0, 1, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), gwordwrap, 1, 0, 1, 1);
		gtk_grid_attach(GTK_GRID(viewgrid), gtk_label_new("Font"), 0, 2, 1, 1);
			gtk_grid_attach(GTK_GRID(viewgrid), gfont, 1, 2, 1, 1);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), filegrid, gtk_label_new("File"));
		gtk_grid_attach(GTK_GRID(filegrid), gtk_label_new("Default Workspace"), 0, 0, 1, 1);
			gtk_grid_attach(GTK_GRID(filegrid), gdefworkspace, 1, 0, 1, 1);
		gtk_grid_attach(GTK_GRID(filegrid), gtk_label_new("Permit Overwrite"), 0, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(filegrid), gpermitoverwrite, 1, 1, 1, 1);
		gtk_grid_attach(GTK_GRID(filegrid), gtk_label_new("Autosave"), 0, 2, 1, 1);
			gtk_grid_attach(GTK_GRID(filegrid), gautosave, 1, 2, 1, 1);
		gautosaverate_label = gtk_label_new("Autosave Rate");
			gtk_grid_attach(GTK_GRID(filegrid), gautosaverate_label, 0, 3, 1, 1);
			gtk_grid_attach(GTK_GRID(filegrid), gautosaverate, 1, 3, 1, 1);

	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), windowgrid, gtk_label_new("Window"));
		gtk_grid_attach(GTK_GRID(windowgrid), gtk_label_new("NOTE: These options require restarting SGNotes"), 0, 0, 2, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), gtk_label_new("Use CSD Decoration"), 0, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(windowgrid), gusecsd, 1, 1, 1, 1);
		gtk_grid_attach(GTK_GRID(windowgrid), gtk_label_new("Resizable elements\n(Experimental)"), 0, 2, 1, 1);
			gtk_grid_attach(GTK_GRID(windowgrid), gresizablewidgets, 1, 2, 1, 1);

		gtk_box_pack_end(GTK_BOX(applybox), applybtn, FALSE, FALSE, 2);
		gtk_box_pack_end(GTK_BOX(applybox), defbtn, FALSE, FALSE, 2);

	GtkWidget *confbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_box_pack_start(GTK_BOX(confbox), notebook, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(confbox), applybox, FALSE, FALSE, 2);

	gtk_container_add(GTK_CONTAINER(dialog), confbox);

	gtk_widget_show_all(dialog);

	if (autosave == 0)
	{
		gtk_widget_hide(gautosaverate);
		gtk_widget_hide(gautosaverate_label);
	}

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gautosave), autosave);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gwordwrap), wordwrap);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gpermitoverwrite), permitoverwrite);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gusecsd), usecsd);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gresizablewidgets), resizablewidgets);

	gchar *selectedfont = g_strdup_printf("%s %s %d", fontfamily, fontstyle, fontsize);

	gtk_font_chooser_set_font(GTK_FONT_CHOOSER(gfont), selectedfont);

	int num_items = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(GTK_COMBO_BOX(gdefworkspace)), NULL);
	for (int i = 0; i < num_items; i++)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(gdefworkspace), i);
		const gchar *active_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gdefworkspace));
		if (g_strcmp0(active_text, defworkspace) == 0)
		{
			break;
		}
	}

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gautosaverate), autosaverate);

	g_signal_connect(gautosave, "clicked", G_CALLBACK(toggleautosave), NULL);

	g_signal_connect(applybtn, "clicked", G_CALLBACK(updateconf), GINT_TO_POINTER(0));
	g_signal_connect(defbtn, "clicked", G_CALLBACK(updateconf), GINT_TO_POINTER(1));
}
