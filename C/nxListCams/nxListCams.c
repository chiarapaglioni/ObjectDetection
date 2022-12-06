#include "nxLib.h"

#include "../common/example.h"

#include <stddef.h>

int main(void)
{
	int error;

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

	return 0;
}
