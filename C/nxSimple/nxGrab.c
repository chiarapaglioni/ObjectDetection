#include "nxLib.h"

#include "../common/example.h"

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

double computeAverageZ(float* pointMap, int width, int height)
{
	double zAverage = 0;
	int zCount = 0;
	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			// Get Z coordinate of the point at image pixel (x,y)
			float pZ = pointMap[(y * width + x) * 3 + 2];
			if (isnan(pZ)) continue; // NaN values indicate missing pixels
			zAverage += (double)pZ;
			zCount++;
		}
	}
	if (zCount) zAverage /= zCount;
	return zAverage;
}

#define MAXSTR 1024
int main()
{
	int width, height, error;
	float* pointMap = 0;
	int pointMapSize = 0;
	char serial[64];
	char path[128];

	printf("This is a simple C console demo for grabbing 3D data via NxLib\n\n");
	printf("Opening NxLib and waiting for cameras to be detected\n");
	nxLibInitialize(&error, 1);
	exit_on_error(error);

	/* Identify first openable Ensenso camera */
	char const* types[2] = {valStereo, valStructuredLight};
	if (!get_available_camera(serial, 2, types)) {
		printf("No depth camera found to open!\n");
		exit(EXIT_FAILURE);
	}

	// Set parameters for the open command
	printf("Opening camera %s\n", serial);
	nxLibSetString(&error, "/Execute/Parameters/Cameras", serial);
	exit_on_error(error);
	nxLibSetString(&error, "/Execute/Command", "Open");
	exit_on_error(error);
	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 1);
	exit_on_error(error);

	// Set the exposure to 5ms if the lib item exists (file cameras do not have them)
	sprintf(path, "/Cameras/%s/Parameters/Capture/AutoExposure", serial);
	nxLibSetBool(&error, path, 0);
	if (error != NxLibCannotCreateItem) exit_on_error(error);

	sprintf(path, "/Cameras/%s/Parameters/Capture/Exposure", serial);
	nxLibSetDouble(&error, path, 5);
	if (error != NxLibCannotCreateItem) exit_on_error(error);

	// Execute the 'Capture', 'ComputeDisparityMap' and 'ComputePointMap' commands
	printf("Grabbing an image\n");
	nxLibSetString(&error, "/Execute/Command", "Capture");
	exit_on_error(error);
	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 1);
	exit_on_error(error);

	printf("Computing the disparity map\n");
	nxLibSetString(&error, "/Execute/Command", "ComputeDisparityMap");
	exit_on_error(error);
	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 1);
	exit_on_error(error);

	printf("Generating point map from disparity map\n");
	nxLibSetString(&error, "/Execute/Command", "ComputePointMap");
	exit_on_error(error);
	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 1);
	exit_on_error(error);

	// Get info about the computed point map
	sprintf(path, "/Cameras/%s/Images/PointMap", serial);
	nxLibGetBinaryInfo(&error, path, &width, &height, 0, 0, 0, 0);
	exit_on_error(error);
	pointMapSize = 3 * width * height * sizeof(pointMap[0]);
	pointMap = (float*)malloc(pointMapSize);
	nxLibGetBinary(&error, path, pointMap, pointMapSize, 0, 0);
	if (is_error(error)) {
		free(pointMap);
		exit_on_error(error);
	}

	// Compute average Z value
	printf("The average z value in the point map is %.1fmm.\n", computeAverageZ(pointMap, width, height));
	free(pointMap);

	printf("Closing camera\n");
	nxLibSetString(&error, "/Execute/Command", "Close");
	exit_on_error(error);
	nxLibWaitForType(&error, "/Execute/Command", NxLibItemTypeNull, 1);
	exit_on_error(error);
	printf("Closing NxLib\n");
	nxLibFinalize(&error);
	exit_on_error(error);

#ifdef _WIN32
	printf("Press enter to quit.\n");
	getchar();
#endif
	return 0;
}
