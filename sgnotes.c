#include "sgnotes.h"

int main(int argc, char *argv[])
{
	const char* env_sgcsd = getenv("SGCSD");
	home_dir = getenv("HOME");
	nocsd = (env_sgcsd != NULL) ? atoi(env_sgcsd) == 0 : 0;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--nocsd") == 0)
		{
			nocsd = 1;
			printf("CSD Disabled, using fallback display\n");
		}
		else if (strcmp(argv[i], "--") != 0)
		{
			strncpy(current_folder, argv[i], sizeof(current_folder) - 1);
			current_folder[sizeof(current_folder) - 1] = '\0';
		}
	}
	printf("current_folder: %s\n", current_folder);

	readconf();
	gtk_init(&argc, &argv);
	create_window();
}
