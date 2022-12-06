#include "nxLibDynamic.h"

#include "../common/example.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#	include <ShlObj.h>
#	include <Windows.h>
#endif

int main(void)
{
	int error;
	NXLIBSTR lib = 0;

#ifdef _WIN32
	char libPath[88];
	wchar_t* utf16path = 0;
	SHGetKnownFolderPath(&FOLDERID_System, 0, 0, &utf16path);
	sprintf(libPath, "%ws\\NxLib64.dll", utf16path);
	lib = &libPath[0];
#else
	lib = "/usr/lib/libNxLib.so";
#endif

	nxLibLoadDynamic(&error, lib);
	exit_on_error(error);

	/* Initialize NxLib and enumerate cameras */
	nxLibInitialize(&error, NXLIBTRUE);
	exit_on_error(error);

	/* Print information for all cameras in the tree */
	print_list_of_cameras();

#ifdef _WIN32
	printf("Press enter to quit.\n");
	getchar();
#endif

	/* Close NxLib */
	nxLibFinalize(&error);
	nxLibFreeDynamic(&error);

	return 0;
}
