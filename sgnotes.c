#include "sgnotes.h"
#include "events.h"
#include "editor.h"
#include "fmanip.h"
#include "img.h"

int main(int argc, char *argv[])
{
	int nocsd = 0;
	store = gtk_list_store_new(1, GDK_TYPE_PIXBUF);

	if (argc > 1 && strcmp(argv[1], "--") != 0)
	{
		strncpy(current_folder, argv[1], sizeof(current_folder) - 1);
	}
	else
	{
		strcpy(current_folder, "Default");
	}
	printf("current_folder: %s\n", current_folder);

	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "--nocsd") == 0)
		{
		nocsd = 1;
		printf("CSD Disabled, using fallback display \n");
		}
	}
	showfind = 0;

	readconf();

	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "SGNotes");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_widget_set_size_request(window, 666, 444);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
		theme = gtk_icon_theme_get_default();
		info = gtk_icon_theme_lookup_icon(theme, "accessories-notes", 48, 0);
		if (info != NULL)
		{
			icon = gtk_icon_info_load_icon(info, NULL);
			gtk_window_set_icon(GTK_WINDOW(window), icon);
			g_object_unref(icon);
			g_object_unref(info);
		}
	accel_group = gtk_accel_group_new();
		gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	GtkWidget *headerbar = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);

		GtkWidget *button = gtk_menu_button_new();
		GtkWidget *wicon = gtk_image_new_from_icon_name("accessories-notes", GTK_ICON_SIZE_BUTTON);
		gtk_container_add(GTK_CONTAINER(button), wicon);
		gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), button);

		wintitle = gtk_label_new(NULL);
		gtk_label_set_markup(GTK_LABEL(wintitle), "<b>Notes - SGNotes</b>");
		gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), wintitle);

		GtkWidget *submenu = gtk_menu_new();
			submenu_item1 = gtk_menu_item_new_with_label("Create new note");
			GtkWidget *submenu_item2 = gtk_check_menu_item_new_with_label("Text Wrapping");
			GtkWidget *submenu_item3 = gtk_menu_item_new_with_label("About");
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item1);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item2);

		GtkWidget *submenu_filelist = gtk_menu_new();
			GtkWidget *submenu_filelist_item1 = gtk_menu_item_new_with_label("Create new note");
			submenu_filelist_item2 = gtk_menu_item_new_with_label("Rename Current Note");
			submenu_filelist_item3 = gtk_menu_item_new_with_label("Delete Current Note");
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_filelist_item2);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_filelist_item3);
		GtkWidget *submenu_imglist = gtk_menu_new();
			GtkWidget *submenu_imglist_item1 = gtk_menu_item_new_with_label("Open Picture");
			GtkWidget *submenu_imglist_item2 = gtk_menu_item_new_with_label("Delete Picture");
			submenu_imglist_item3 = gtk_menu_item_new_with_label("Add Picture to current note");
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu_imglist), submenu_imglist_item1);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu_imglist), submenu_imglist_item2);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_imglist_item3);
					if (wraptext == 0)
					{
						gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item2), FALSE);
					}
					else if (wraptext ==1)
					{
						gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item2), TRUE);
					}
					gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item3);

		gtk_widget_show_all(submenu);
		gtk_widget_show_all(submenu_filelist);
		gtk_widget_show_all(submenu_imglist);

		gtk_menu_button_set_popup(GTK_MENU_BUTTON(button), submenu);


	if (nocsd == 0 )
	{
		gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
	}

	// Create grid
	grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), grid);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	
	scrolled_list = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_list), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	scrolled_txt = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_txt), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		list = gtk_list_box_new();
			gtk_container_add(GTK_CONTAINER(scrolled_list), list);
		text_view = gtk_source_view_new();
			gtk_container_add(GTK_CONTAINER(scrolled_txt), text_view);
			if (wraptext == 1)
			{
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
			}
			else if (wraptext == 0)
			{
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
			}
			search_entry = gtk_entry_new();
				next_button = gtk_button_new_with_label("Next");
				prev_button = gtk_button_new_with_label("Previous");

		scrolled_treeview = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_treeview), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_widget_set_size_request(scrolled_treeview, 100, 150);

		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
		GtkSourceLanguageManager *language_manager = gtk_source_language_manager_get_default();
		GtkSourceLanguage *language = gtk_source_language_manager_get_language(language_manager, "markdown");
		gtk_source_buffer_set_language(buffer, language);
		
		rename_button = gtk_button_new_with_label("Rename");
		delete_button = gtk_button_new_with_label("Delete Note");
		pic_button = gtk_button_new_with_label("Add Picture");
		treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
		GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
			gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
			gtk_container_add(GTK_CONTAINER(scrolled_treeview), treeview);

		GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(NULL, renderer, "pixbuf", 0, NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
		gtk_tree_view_column_set_expand(column, TRUE);
		gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

		textbox_grid = gtk_grid_new();
			gtk_grid_attach(GTK_GRID(textbox_grid), search_entry, 0, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(textbox_grid), prev_button, 1, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(textbox_grid), next_button, 2, 1, 1, 1);
				gtk_widget_set_hexpand(search_entry, TRUE);

		gtk_grid_attach(GTK_GRID(grid), scrolled_list, 0, 0, 1, 2);
		gtk_grid_attach(GTK_GRID(grid), scrolled_txt, 1, 0, 1, 1);
		gtk_grid_attach(GTK_GRID(grid), textbox_grid, 1, 1, 1, 1);
		gtk_grid_attach(GTK_GRID(grid), scrolled_treeview, 2, 0, 1, 2);

		gtk_widget_set_vexpand(scrolled_list, TRUE);
		gtk_widget_set_hexpand(scrolled_list, TRUE);
		gtk_widget_set_vexpand(scrolled_txt, TRUE);
		gtk_widget_set_hexpand(scrolled_txt, TRUE);
		

		g_signal_connect(list, "row-selected", G_CALLBACK(on_list_item_selected), NULL);
		g_signal_connect(rename_button, "clicked", G_CALLBACK(on_rename_button_clicked), NULL);
		g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), NULL);
		g_signal_connect(pic_button, "clicked", G_CALLBACK(add_image), NULL);

		g_signal_connect(submenu_item1, "activate", G_CALLBACK(on_submenu_item1_selected), NULL);
		g_signal_connect(submenu_item2, "activate", G_CALLBACK(on_submenu_item2_selected), NULL);
		g_signal_connect(submenu_item3, "activate", G_CALLBACK(on_submenu_item3_selected), NULL);

		g_signal_connect(submenu_filelist_item2, "activate", G_CALLBACK(on_rename_button_clicked), NULL);
		g_signal_connect(submenu_filelist_item3, "activate", G_CALLBACK(on_delete_button_clicked), NULL);
		
		g_signal_connect(submenu_imglist_item1, "activate", G_CALLBACK(on_submenu_imglist_item1_selected), NULL);
		g_signal_connect(submenu_imglist_item2, "activate", G_CALLBACK(on_submenu_imglist_item2_selected), NULL);
		g_signal_connect(submenu_imglist_item3, "activate", G_CALLBACK(add_image), NULL);

	gtk_widget_add_accelerator(submenu_item1, "activate", accel_group, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press), submenu);
	g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), submenu);
	g_signal_connect(list, "button-press-event", G_CALLBACK(on_list_press), NULL);
	g_signal_connect(treeview, "button-press-event", G_CALLBACK(on_treeview_clicked), submenu_imglist);
	g_signal_connect(G_OBJECT(search_entry), "changed", G_CALLBACK(search_entry_changed), NULL);
	g_signal_connect(G_OBJECT(next_button), "clicked", G_CALLBACK(next_button_clicked), NULL);
	g_signal_connect(G_OBJECT(prev_button), "clicked", G_CALLBACK(prev_button_clicked), NULL);
	load_file_list();

	g_timeout_add(100, timeout_callback, NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show_all(window);
	gtk_widget_hide(scrolled_txt);
	gtk_widget_hide(pic_button);
	gtk_widget_hide(textbox_grid);
	gtk_widget_hide(treeview);
	gtk_widget_hide(scrolled_treeview);
	gtk_main();

return 0;
}
