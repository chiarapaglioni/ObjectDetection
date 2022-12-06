#include "nxLib.h"

#include "../common/example.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
	int error;
	int port;
	int n, m;
	char path_to_cam[84];
	char path[128];
	char serial[64];
	char pathn[140];
	char pathm[152];
	int countn, countm;

	/* Initialize NxLib and enumerate cameras */
	nxLibInitialize(&error, NXLIBTRUE);
	exit_on_error(error);

	/* Open TCP port to use NxTreeEdit */
	nxLibOpenTcpPort(&error, 24001, &port);
	exit_on_error(error);

	/* Identify first openable Ensenso camera */
	char const* types[2] = {valStereo, valStructuredLight};

	if (!get_available_camera(serial, 2, types)) {
		printf("No depth camera found to open!\n");
		exit(EXIT_FAILURE);
	}

	printf("Opening camera %s\n", serial);

	/* Open camera with serial number */
	sprintf(path_to_cam, "/Cameras/%s", serial);
	printf("Path to camera: %s\n", path_to_cam);

	nxLibSetString(&error, "/Execute/Parameters/Cameras", serial);
	exit_on_error(error);
	nxLibSetString(&error, "/Execute/Command", "Open");
	exit_on_error(error);
	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 5);
	exit_on_error(error);
	/* Check if command has succeeded: ErrorSymbol not existing */

	/* Access example matrix: */
	/* Calibration/Stereo/Reprojection is a 4x4 matrix */
	sprintf(path, "%s/Calibration/Stereo/Reprojection", path_to_cam);
	printf("Item path: %s\n", path);
	if (nxLibGetType(&error, path) != NxLibItemTypeArray) {
		exit_on_error(error);
		printf("Type of %s is not Array!\n", path);
		return -1;
	}
	countn = nxLibGetCount(&error, path);
	exit_on_error(error);
	printf("Array has %d elements\n", countn);

	for (n = 0; n < countn; n++) {
		/* Check each element: either Number or Array */
		sprintf(pathn, "%s/\\%d", path, n);
		if (nxLibGetType(&error, pathn) == NxLibItemTypeNumber) {
			printf("Array element %d is Number: %f\n", n, nxLibGetDouble(&error, pathn));
			exit_on_error(error);
		} else if (nxLibGetType(&error, pathn) == NxLibItemTypeArray) {
			countm = nxLibGetCount(&error, pathn);
			exit_on_error(error);
			printf("Array element %d has %d elements:", n, countm);
			for (m = 0; m < countm; m++) {
				sprintf(pathm, "%s/\\%d", pathn, m);
				printf(" %f", nxLibGetDouble(&error, pathm));
				exit_on_error(error);
			}
			printf("\n");
		} else {
			printf("Array element %d is not neither Array nor Number!", n);
		}
	}

#ifdef _WIN32
	printf("Press enter to quit.\n");
	getchar();
#endif

	/* Close TCP port */
	nxLibCloseTcpPort(&error);
	exit_on_error(error);

	/* Close NxLib */
	nxLibFinalize(&error);

	return 0;
}
