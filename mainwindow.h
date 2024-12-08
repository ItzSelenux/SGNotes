void on_submenu_item_about_selected(GtkMenuItem *menuitem, gpointer userdata)
{
	dialog = gtk_about_dialog_new();
		window_set_icon(GTK_WINDOW(dialog), program_icon);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SGNotes");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), pver);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "Copyright © 2024 ItsZariep");
	#ifdef WITHSOURCEVIEW
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Simple GTK Notes\nThis build uses GTK SourceView");
	#else
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "Simple GTK Notes");
	#endif
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "https://codeberg.org/ItsZariep/SGNotes");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), "Project WebSite");
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog),GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(dialog),program_icon);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void help_handler(void)
{
	const gchar *link = "https://codeberg.org/ItsZariep/SGNotes/wiki/?action=_pages";

	dialog = gtk_dialog_new_with_buttons("Documentation available on Codeberg", NULL, GTK_DIALOG_MODAL, NULL, NULL);
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	GtkWidget *info_image = gtk_image_new_from_icon_name("dialog-information", GTK_ICON_SIZE_DIALOG);
	GtkWidget *label = gtk_label_new("Documentation available at:");

	gchar *markup = g_strdup_printf("<a href=\"%s\">%s</a>", link, link);
	GtkWidget *link_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(link_label), markup);
	g_free(markup);

	gtk_label_set_use_markup(GTK_LABEL(link_label), TRUE);
	gtk_label_set_use_underline(GTK_LABEL(link_label), TRUE);

	gtk_box_pack_start(GTK_BOX(box), info_image, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), link_label, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), box);
	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void create_window(void)
{
	program_icon_names = g_ptr_array_new();
	g_ptr_array_add(program_icon_names, "sgnotes");
	g_ptr_array_add(program_icon_names, "accessories-notes");
	g_ptr_array_add(program_icon_names, "stock_notes");
	g_ptr_array_add(program_icon_names, "accessories-text-editor");

	program_icon = probe_icons_from_theme(program_icon_names);
	imglist_store = gtk_list_store_new(1, GDK_TYPE_PIXBUF);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_widget_set_size_request(window, 666, 444);
	g_signal_connect(window, "delete-event", G_CALLBACK(quit_handler), NULL);

	window_set_icon(GTK_WINDOW(window), program_icon);

	accel_group = gtk_accel_group_new();
		gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	GtkWidget *headerbar = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);

	GtkWidget *mainvbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	GtkWidget *submenu = gtk_menu_new();
	submenu_item_workspace = gtk_menu_item_new_with_label("Switch workspace");

	GtkWidget *submenu_item_settings = gtk_menu_item_new_with_label("Preferences");
	GtkWidget *submenu_item_about = gtk_menu_item_new_with_label("About");
	GtkWidget *submenu_item_onlinehelp = gtk_menu_item_new_with_label("Online Help");

	GtkWidget *submenu_filelist = gtk_menu_new();
	submenu_item_newnote = gtk_menu_item_new_with_label("Create new note");
	submenu_item_save = gtk_menu_item_new_with_label("Save changes");
	submenu_filelist_item2 = gtk_menu_item_new_with_label("Rename Current Note");
	submenu_filelist_item3 = gtk_menu_item_new_with_label("Delete Current Note");
	submenu_item_closefile = gtk_menu_item_new_with_label("Close");
	GtkWidget *submenu_item_quit = gtk_menu_item_new_with_label("Quit");

	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_item_workspace);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_item_newnote);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_item_save);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_filelist_item2);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_filelist_item3);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_item_closefile);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_filelist), submenu_item_quit);
	GtkWidget *submenu_edit = gtk_menu_new();
	submenu_imglist_item3 = gtk_menu_item_new_with_label("Add Picture to current note");
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_edit), submenu_imglist_item3);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu_edit), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_edit), submenu_item_settings);

	GtkWidget *submenu_view = gtk_menu_new();
	submenu_item_wordwrap = gtk_check_menu_item_new_with_label("Word Wrap");
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item_wordwrap), wordwrap);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_view), submenu_item_wordwrap);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu_view), gtk_separator_menu_item_new());
	submenu_item_zoomin = gtk_menu_item_new_with_label("Zoom In");
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_view), submenu_item_zoomin);
	submenu_item_zoomout = gtk_menu_item_new_with_label("Zoom Out");
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_view), submenu_item_zoomout);
	submenu_item_zoomreset = gtk_menu_item_new_with_label("Zoom Reset");
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_view), submenu_item_zoomreset);

	GtkWidget *submenu_help = gtk_menu_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_help), submenu_item_onlinehelp);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu_help), submenu_item_about);


	GtkWidget *submenu_item_edit = gtk_menu_item_new_with_label("Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu_item_edit), submenu_edit);

	GtkWidget *submenu_item_file = gtk_menu_item_new_with_label("File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu_item_file), submenu_filelist);
	GtkWidget *submenu_item_view = gtk_menu_item_new_with_label("View");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu_item_view), submenu_view);
	GtkWidget *submenu_item_help = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu_item_help), submenu_help);

	GtkWidget *menu_bar = gtk_menu_bar_new();

	gtk_widget_show_all(submenu);

	if (nocsd == 1)
	{
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), submenu_item_file);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), submenu_item_edit);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), submenu_item_view);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), submenu_item_help);
		gtk_widget_show_all(submenu);
		gtk_box_pack_start(GTK_BOX(mainvbox), menu_bar, FALSE, FALSE, 0);
	}
	else
	{
		mainbutton = gtk_menu_button_new();
			GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(theme, program_icon, 64, GTK_ICON_LOOKUP_USE_BUILTIN, NULL);
			GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 16, 16, GDK_INTERP_BILINEAR);
			GtkWidget *wicon = gtk_image_new_from_pixbuf(scaled_pixbuf);
		gtk_container_add(GTK_CONTAINER(mainbutton), wicon);
		gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), mainbutton);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item_file);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item_edit);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item_view);
		gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item_help);
		gtk_widget_show_all(submenu);
		gtk_menu_button_set_popup(GTK_MENU_BUTTON(mainbutton), submenu);
		gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
		gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	}

	GtkWidget *submenu_imglist = gtk_menu_new();
		GtkWidget *submenu_imglist_item1 = gtk_menu_item_new_with_label("Open Picture");
		GtkWidget *submenu_imglist_item2 = gtk_menu_item_new_with_label("Delete Picture");
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu_imglist), submenu_imglist_item1);
			gtk_menu_shell_append(GTK_MENU_SHELL(submenu_imglist), submenu_imglist_item2);
	gtk_widget_show_all(submenu_imglist);

	grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), mainvbox);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

	scrolled_list = gtk_scrolled_window_new(NULL, NULL);
		//fixme need to handle wordwrap correctly, setting horizontal scrolling is a temporal solution
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_list), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	scrolled_txt = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_txt), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		filelist_renderer = gtk_cell_renderer_text_new();
			//g_object_set(filelist_renderer, "wrap-width", 64, NULL);
			//g_object_set(filelist_renderer, "wrap-mode", 2, NULL);
		filelist = gtk_tree_view_new();
			gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(filelist), TRUE);
			gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(filelist), FALSE);
			gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(filelist), FALSE);

			gtk_container_add(GTK_CONTAINER(scrolled_list), filelist);
		text_view = gtk_source_view_new();
			gtk_container_add(GTK_CONTAINER(scrolled_txt), text_view);
			if (wordwrap == 1)
			{
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
			}
			else if (wordwrap == 0)
			{
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
			}
			search_entry = gtk_entry_new();
				next_button = gtk_button_new_with_label("Next");
				prev_button = gtk_button_new_with_label("Previous");

		GtkCssProvider *provider = gtk_css_provider_new();
		gchar *css = g_strdup_printf("textview { font-size: %dpt; }", fontsize);
		gtk_css_provider_load_from_data(provider, css, -1, NULL);
		g_free(css);

		GtkStyleContext *currentcontext = gtk_widget_get_style_context(GTK_WIDGET(text_view));
		gtk_style_context_add_provider(currentcontext, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		g_object_unref(provider);

		scrolled_treeview = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_treeview), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
		gtk_widget_set_size_request(scrolled_treeview, 100, 150);

		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
		#ifdef WITHSOURCEVIEW
			GtkSourceLanguageManager *language_manager = gtk_source_language_manager_get_default();
			GtkSourceLanguage *language = gtk_source_language_manager_get_language(language_manager, "markdown");
			gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(buffer), language);
		#endif
		rename_button = gtk_button_new_with_label("Rename");
		delete_button = gtk_button_new_with_label("Delete Note");
		pic_button = gtk_button_new_with_label("Add Picture");
		treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(imglist_store));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
			gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
			gtk_container_add(GTK_CONTAINER(scrolled_treeview), treeview);

		GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(NULL, renderer, "pixbuf", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
		gtk_tree_view_column_set_sizing(column, 10);
		gtk_tree_view_column_set_expand(column, TRUE);
		gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

		sidebar_left = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

		imgbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
			gtk_box_pack_start(GTK_BOX(imgbox), scrolled_treeview, TRUE, TRUE, 0);
			gtk_box_pack_end(GTK_BOX(imgbox), pic_button, FALSE, FALSE, 0);

		textbox_grid = gtk_grid_new();
			gtk_grid_attach(GTK_GRID(textbox_grid), search_entry, 0, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(textbox_grid), prev_button, 1, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(textbox_grid), next_button, 2, 1, 1, 1);
				gtk_widget_set_hexpand(search_entry, TRUE);

		textbox_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
			gtk_container_add(GTK_CONTAINER(textbox_container), scrolled_txt);
			gtk_container_add(GTK_CONTAINER(textbox_container), textbox_grid);

		if (resizablewidgets)
		{
			paned_horiz1 = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
				gtk_paned_pack1(GTK_PANED(paned_horiz1), scrolled_list, TRUE, FALSE);
			paned_horiz2 = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
				gtk_paned_pack1(GTK_PANED(paned_horiz2), textbox_container, TRUE, FALSE);
				gtk_paned_pack2(GTK_PANED(paned_horiz2), imgbox, TRUE, FALSE); 
				gtk_paned_pack2(GTK_PANED(paned_horiz1), paned_horiz2, TRUE, FALSE);

			gtk_container_add(GTK_CONTAINER(mainvbox), paned_horiz1);
		}
		else
		{
			gtk_grid_attach(GTK_GRID(grid), scrolled_list, 0, 0, 1, 2);
			gtk_grid_attach(GTK_GRID(grid), textbox_container, 1, 0, 1, 1);
			gtk_grid_attach(GTK_GRID(grid), imgbox, 2, 0, 1, 2);
			gtk_container_add(GTK_CONTAINER(mainvbox), grid);
		}

		gtk_widget_set_vexpand(scrolled_list, TRUE);
		gtk_widget_set_hexpand(scrolled_list, TRUE);
		gtk_widget_set_vexpand(scrolled_txt, TRUE);
		gtk_widget_set_hexpand(scrolled_txt, TRUE);

		g_signal_connect(filelist, "row-activated", G_CALLBACK(on_filelist_item_selected), NULL);
		g_signal_connect(filelist, "button-press-event", G_CALLBACK(filelist_element_showmenu), NULL);
		g_signal_connect(buffer, "changed", G_CALLBACK(togglesave), GINT_TO_POINTER(0));
		g_signal_connect(rename_button, "clicked", G_CALLBACK(on_rename_button_clicked), NULL);
		g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), NULL);
		g_signal_connect(pic_button, "clicked", G_CALLBACK(add_image), NULL);

		g_signal_connect(submenu_item_workspace, "activate", G_CALLBACK(on_submenu_item_workspace_selected), NULL);
		g_signal_connect(submenu_item_newnote, "activate", G_CALLBACK(submenu_item_newnote_selected), NULL);
		g_signal_connect(submenu_item_save, "activate", G_CALLBACK(timeout_callback), NULL);
		g_signal_connect(submenu_item_closefile, "activate", G_CALLBACK(on_close_file), NULL);
		g_signal_connect(submenu_item_quit, "activate", G_CALLBACK(quit_handler), NULL);
		g_signal_connect(submenu_item_about, "activate", G_CALLBACK(on_submenu_item_about_selected), NULL);
		g_signal_connect(submenu_item_onlinehelp, "activate", G_CALLBACK(help_handler), "HELP");
		g_signal_connect(submenu_item_settings, "activate", G_CALLBACK(showcfg), NULL);

		g_signal_connect(submenu_filelist_item2, "activate", G_CALLBACK(on_rename_button_clicked), NULL);
		g_signal_connect(submenu_filelist_item3, "activate", G_CALLBACK(on_delete_button_clicked), NULL);

		g_signal_connect(submenu_item_wordwrap, "activate", G_CALLBACK(updateconf), GINT_TO_POINTER(0));

		g_signal_connect(submenu_imglist_item1, "activate", G_CALLBACK(on_submenu_imglist_item1_selected), NULL);
		g_signal_connect(submenu_imglist_item2, "activate", G_CALLBACK(on_submenu_imglist_item2_selected), NULL);
		g_signal_connect(submenu_imglist_item3, "activate", G_CALLBACK(add_image), NULL);

	//gtk_widget_add_accelerator(submenu_item_workspace, "activate", accel_group, GDK_KEY_Tab, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_newnote, "activate", accel_group, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_save, "activate", accel_group, GDK_KEY_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_closefile, "activate", accel_group, GDK_KEY_W, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_quit, "activate", accel_group, GDK_KEY_Q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_zoomin, "activate", accel_group, GDK_KEY_equal, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_zoomout, "activate", accel_group, GDK_KEY_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_zoomreset, "activate", accel_group, GDK_KEY_0, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(submenu_item_wordwrap, "activate", accel_group, GDK_KEY_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	g_signal_connect(submenu_item_zoomin, "activate", G_CALLBACK(adjust_font_size), GINT_TO_POINTER(2));
	g_signal_connect(submenu_item_zoomout, "activate", G_CALLBACK(adjust_font_size), GINT_TO_POINTER(-2));
	g_signal_connect(submenu_item_zoomreset, "activate", G_CALLBACK(adjust_font_size), GINT_TO_POINTER(0));

	g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press), submenu);
	g_signal_connect(G_OBJECT(window), "scroll-event", G_CALLBACK(on_scroll_event), text_view);
	g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), submenu);
	g_signal_connect(filelist, "button-press-event", G_CALLBACK(on_list_press), NULL);
	g_signal_connect(treeview, "button-press-event", G_CALLBACK(on_treeview_clicked), submenu_imglist);
	g_signal_connect(G_OBJECT(search_entry), "changed", G_CALLBACK(search_entry_changed), NULL);
	g_signal_connect(G_OBJECT(next_button), "clicked", G_CALLBACK(next_button_clicked), NULL);
	g_signal_connect(G_OBJECT(prev_button), "clicked", G_CALLBACK(prev_button_clicked), NULL);
	load_file_list();


	if (autosave)
	{
		if (autosaverate == 0)
		{
			timeout_id = g_timeout_add(100, timeout_callback, NULL);
		}
		else
		{
			timeout_id = g_timeout_add_seconds(autosaverate, timeout_callback, NULL);
		}
	}

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show_all(window);
	snprintf(markup_buffer, sizeof(markup_buffer), "%s - SGNotes", current_workspace);
	gtk_window_set_title(GTK_WINDOW(window), markup_buffer);

	gtk_widget_hide(textbox_container);
	gtk_widget_hide(imgbox);

	gtk_widget_set_sensitive(submenu_imglist_item3, FALSE);
	gtk_widget_set_sensitive(submenu_filelist_item2, FALSE);
	gtk_widget_set_sensitive(submenu_filelist_item3, FALSE);
	gtk_widget_set_sensitive(submenu_item_save, FALSE);
	gtk_widget_set_sensitive(submenu_item_closefile, FALSE);
	gtk_widget_set_sensitive(submenu_item_zoomin, FALSE);
	gtk_widget_set_sensitive(submenu_item_zoomreset, FALSE);
	gtk_widget_set_sensitive(submenu_item_zoomout, FALSE);
	updateuistyle();
	gtk_main();
}

void restart_program(void)
{
	current_file[0] = '\0';
	gtk_widget_destroy(window);
	readconf();
	create_window();
}
