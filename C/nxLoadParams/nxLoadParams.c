#include "nxLib.h"

#include "../common/example.h"

#include <stdio.h>
#include <string.h>

#define STR_MAX 10000

int main(void)
{
	int error;
	FILE* file;
	int readchar;
	char serial[64];
	char path[128];
	char str[STR_MAX + 1];
	int num_char;

	/* Initialize NxLib and enumerate cameras */
	nxLibInitialize(&error, NXLIBTRUE);
	exit_on_error(error);

	/* Identify first openable Ensenso camera */
	char const* types[2] = {valStereo, valStructuredLight};
	if (!get_available_camera(serial, 2, types)) {
		printf("No depth camera found to open!\n");
		exit(EXIT_FAILURE);
	}

	exit_on_error(error);
	printf("Opening camera %s\n", serial);

	nxLibSetString(&error, "/Execute/Parameters/Cameras", serial);
	exit_on_error(error);

	nxLibSetString(&error, "/Execute/Command", "Open");
	exit_on_error(error);

	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 1);
	exit_on_error(error);

	/* Read the JSON file*/
	file = fopen("ensenso_params.json", "r");
	if (NULL == file) {
		printf("Could not open file ensenso_params.json");
		exit(EXIT_FAILURE);
	}
	num_char = 0;
	while (1) {
		readchar = getc(file);
		if (EOF == readchar || STR_MAX == num_char) {
			str[num_char] = '\0';
			break;
		}
		str[num_char++] = (char)readchar;
	}
	fclose(file);

	/* Set the JSON value/tree */
	sprintf(path, "/Cameras/%s/Parameters", serial);
	nxLibSetJson(&error, path, str, NXLIBTRUE);
	exit_on_error(error);

#ifdef _WIN32
	printf("Press enter to quit.\n");
	getchar();
#endif

	/* Close NxLib */
	nxLibFinalize(&error);

	return 0;
}
