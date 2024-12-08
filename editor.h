void find (GtkTextView *input, const gchar *text, GtkTextIter *iter)
{
	GtkTextIter mstart, mend;
	gboolean found;
	GtkTextBuffer *currentbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
	GtkTextMark *last_pos;

	found = gtk_text_iter_forward_search (iter, text, 0, &mstart, &mend, NULL);

	if (found)
	{
		gtk_text_buffer_select_range (currentbuffer, &mstart, &mend);
		last_pos = gtk_text_buffer_create_mark (buffer, "last_pos", 
		&mend, FALSE);
		gtk_text_view_scroll_mark_onscreen (input, last_pos);
	}
}

void search_entry_changed(GtkEditable *editable, gpointer user_data)
{
	const gchar *text;
	GtkTextBuffer *currentbuffer = NULL;
	GtkTextIter iter;

	text = gtk_entry_get_text(GTK_ENTRY(editable));

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	gtk_text_buffer_get_start_iter(currentbuffer, &iter);

	find(GTK_TEXT_VIEW(text_view), text, &iter);
}

void next_button_clicked(GtkWidget *input)
{
	const gchar *text;
	GtkTextBuffer *currentbuffer = NULL;
	GtkTextMark *last_pos;
	GtkTextIter iter;

	text = gtk_entry_get_text(GTK_ENTRY(search_entry));

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

	last_pos = gtk_text_buffer_get_mark(currentbuffer, "last_pos");
	if (last_pos == NULL)
	{
		return;
	}

	gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);
	find(GTK_TEXT_VIEW(text_view), text, &iter);
}

void prev_button_clicked(GtkWidget *input)
{
	const gchar *text;
	GtkTextBuffer *currentbuffer;
	GtkTextIter iter, start, end;

	text = gtk_entry_get_text(GTK_ENTRY(search_entry));
	currentbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	last_found = gtk_text_buffer_get_mark(buffer, "last_pos");

	if (last_found != NULL)
	{
		gtk_text_buffer_get_iter_at_mark(currentbuffer, &iter, last_found);

		if (gtk_text_iter_backward_search(&iter, text, 0, &start, &end, NULL))
		{
			gtk_text_buffer_select_range(currentbuffer, &start, &end);
			gtk_text_mark_set_visible(last_found, FALSE);
			last_found = gtk_text_buffer_create_mark(currentbuffer, "last_pos", &start, FALSE);
			gtk_text_mark_set_visible(last_found, TRUE);
			gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(text_view), last_found, 0.0, TRUE, 0.5, 0.5);
		}
	}
}

void load_file_content(const char *filename)
{
	gchar file_path[8192];
	snprintf(file_path, sizeof(file_path), "%s%s%s/%s", home_dir, notes_dir, current_workspace, filename);
	FILE *file = fopen(file_path, "r");
	if (file)
	{
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), "", -1);

		char *line = NULL;
		size_t len = 0;
		ssize_t read;

		while ((read = getline(&line, &len, file)) != -1)
		{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer), line, read);
		}

		fclose(file);

		if (line)
		{
			free(line);
		}

		strncpy(current_file, filename, sizeof(current_file));
	}
}
