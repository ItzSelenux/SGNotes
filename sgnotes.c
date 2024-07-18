#include "sgnotes.h"

int main(int argc, char *argv[])
{
	const char* env_sgcsd = getenv("SGCSD");
	home_dir = getenv("HOME");
	nocsd = (env_sgcsd != NULL) ? atoi(env_sgcsd) == 0 : 0;

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

	readconf();
	gtk_init(&argc, &argv);
	create_window();
}
