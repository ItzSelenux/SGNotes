#include "sgnotes.h"

int main(int argc, char *argv[])
{
	const gchar* env_sgcsd = getenv("SGCSD");
	home_dir = getenv("HOME");
	notes_dir = "/.local/share/sgnotes/";
	snprintf(workspaces_path, sizeof(workspaces_path), "%s%s", home_dir, notes_dir);

	nocsd = (env_sgcsd != NULL) ? atoi(env_sgcsd) == 0 : 0;
	fcsd = (env_sgcsd != NULL) ? 1 : 0;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--nocsd") == 0)
		{
			nocsd = 1;
			fcsd = 1;
			g_print("CSD Disabled, using fallback display");
		}
		else if (strcmp(argv[i], "--") != 0)
		{
			strncpy(current_workspace, argv[i], sizeof(current_workspace) - 1);
			current_workspace[sizeof(current_workspace) - 1] = '\0';
		}
	}
	g_print("current_workspace: %s\n", current_workspace);

	readconf();
	gtk_init(&argc, &argv);
	create_window();
}
