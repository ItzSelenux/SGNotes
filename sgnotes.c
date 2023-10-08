#include "sgnotes.h"

int main(int argc, char *argv[])
{
int nocsd = 0;

if (argc > 0)
{
	pm = argv[0];
}
else
{
	pm = "sgnotes";
}

for(int i = 1; i < argc; i++)
{
	if(strcmp(argv[i], "--nocsd") == 0)
	{
	nocsd = 1;
	printf("CSD Disabled, using fallback display \n");
	}
}

readconf();

gtk_init(&argc, &argv);
// Create the main window
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
	//accelerator header
	accel_group = gtk_accel_group_new();
		gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

	// Create the header bar
	GtkWidget *headerbar = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);

		// Create the button with an icon
		GtkWidget *button = gtk_menu_button_new();
		GtkWidget *wicon = gtk_image_new_from_icon_name("accessories-notes", GTK_ICON_SIZE_BUTTON);
		gtk_container_add(GTK_CONTAINER(button), wicon);
		gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), button);

		// Create the title
		GtkWidget *label = gtk_label_new(NULL);
		gtk_label_set_markup(GTK_LABEL(label), "<b>Notes - SGNotes</b>");
		gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), label);

		// Create the submenu
		GtkWidget *submenu = gtk_menu_new();
			// Create the submenu items
			submenu_item1 = gtk_menu_item_new_with_label("Create new note");
			GtkWidget *submenu_item2 = gtk_check_menu_item_new_with_label("Text Wrapping");
			GtkWidget *submenu_item3 = gtk_menu_item_new_with_label("About");
				// Add the submenu items to the submenus
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item1);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item2);
				gtk_menu_shell_append(GTK_MENU_SHELL(submenu), submenu_item3);
					if (wraptext == 0)
					{
						gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item2), FALSE);
					}
					else if (wraptext ==1)
					{
						gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(submenu_item2), TRUE);
					}

		// Show all the submenu items
		gtk_widget_show_all(submenu);

		// Connect the button to the submenu
		gtk_menu_button_set_popup(GTK_MENU_BUTTON(button), submenu);


	if (nocsd == 0 )
	{
		// Add the header bar to the main window
		gtk_window_set_titlebar(GTK_WINDOW(window), headerbar);
	}
	
	// Create grid
	grid = gtk_grid_new();
//	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
//	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_container_add(GTK_CONTAINER(window), grid);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	
	scrolled_list = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_list), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	scrolled_txt = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_txt), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		list = gtk_list_box_new();
			gtk_container_add(GTK_CONTAINER(scrolled_list), list);
		text_view = gtk_text_view_new();
			gtk_container_add(GTK_CONTAINER(scrolled_txt), text_view);
			if (wraptext == 1)
			{
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
			}
			else if (wraptext == 0)
			{
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_NONE);
			}
		buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
		//save_button = gtk_button_new_with_label("Save");
		rename_button = gtk_button_new_with_label("Rename");
		delete_button = gtk_button_new_with_label("Delete Note");

		textbox_grid = gtk_grid_new();
		gtk_grid_set_column_homogeneous(GTK_GRID(textbox_grid), TRUE);
			//gtk_grid_attach(GTK_GRID(textbox_grid), save_button, 0, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(textbox_grid), rename_button, 0, 1, 1, 1);
			gtk_grid_attach(GTK_GRID(textbox_grid), delete_button, 1, 1, 1, 1);

		gtk_grid_attach(GTK_GRID(grid), scrolled_list, 0, 1, 1, 2);
		gtk_grid_attach(GTK_GRID(grid), scrolled_txt, 1, 1, 1, 1);
		gtk_grid_attach(GTK_GRID(grid), textbox_grid, 1, 2, 1, 1);

		gtk_widget_set_vexpand(scrolled_list, TRUE);
		gtk_widget_set_hexpand(scrolled_list, TRUE);
		gtk_widget_set_vexpand(scrolled_txt, TRUE);
		gtk_widget_set_hexpand(scrolled_txt, TRUE);

		g_signal_connect(list, "row-selected", G_CALLBACK(on_list_item_selected), NULL);
		//g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), NULL);
		g_signal_connect(rename_button, "clicked", G_CALLBACK(on_rename_button_clicked), NULL);
		g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), NULL);

		g_signal_connect(submenu_item1, "activate", G_CALLBACK(on_submenu_item1_selected), NULL);
		g_signal_connect(submenu_item2, "activate", G_CALLBACK(on_submenu_item2_selected), NULL);
		g_signal_connect(submenu_item3, "activate", G_CALLBACK(on_submenu_item3_selected), NULL);


	gtk_widget_add_accelerator(submenu_item1, "activate", accel_group, GDK_KEY_N, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	//gtk_widget_add_accelerator(save_button, "activate", accel_group, GDK_KEY_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press), submenu);
	load_file_list();

	g_timeout_add(100, timeout_callback, NULL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show_all(window);
	
	gtk_widget_hide(textbox_grid);
	gtk_widget_hide(scrolled_txt);
	gtk_main();

return 0;
}
