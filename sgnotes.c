#include "sgnotes.h"

int main(int argc, char *argv[])
{
	const gchar *env_sgcsd = g_getenv("SGCSD");
	home_dir = g_get_home_dir();
	notes_dir = "/.local/share/sgnotes/";
	workspaces_path = g_build_filename(home_dir, notes_dir, NULL);

	nocsd = (env_sgcsd != NULL) ? atoi(env_sgcsd) == 0 : 0;
	fcsd = (env_sgcsd != NULL) ? 1 : 0;

	context = g_option_context_new(NULL);
	g_option_context_add_main_entries(context, entries, NULL);

	if (!g_option_context_parse(context, &argc, &argv, &error))
	{
		g_printerr("Error parsing options: %s\n", error->message);
		g_error_free(error);
		g_option_context_free(context);
		return 1;
	}

	if (remaining_args && remaining_args[0])
	{
		g_strlcpy(current_workspace, remaining_args[0], sizeof(current_workspace));
		initialized = 1;
	}

	if (nocsd)
	{fcsd = TRUE;}

	g_option_context_free(context);
	g_strfreev(remaining_args); 

	readconf();
	gtk_init(&argc, &argv);
	create_window();
}
