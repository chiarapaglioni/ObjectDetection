#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAYBE_UNUSED
#	if defined(_WIN32) || defined(__WINDOWS__)
#		define MAYBE_UNUSED
#	else /* defined (_WIN32) */
#		define MAYBE_UNUSED __attribute__((unused))
#	endif
#endif

MAYBE_UNUSED static int is_error(int error)
{
	return error != NxLibOperationSucceeded;
}

MAYBE_UNUSED static void exit_on_error(int error)
{
	if (is_error(error)) {
		printf("NxLib error: %s\n", nxLibTranslateReturnCode(error));
#ifdef _WIN32
		printf("Press enter to quit.\n");
		getchar();
#endif
		exit(EXIT_FAILURE);
	}
}

MAYBE_UNUSED static int get_available_camera(char* out_serial, int num_types, char const* const* valid_types)
{
	char path[128];
	char path_to_cams[64];
	char cameratype[64];
	char serial[64];
	int error;
	int num_cams;
	int camera_counter;
	int type_counter;

	int is_camera_found = 0;
	strcpy(path_to_cams, "/Cameras");
	num_cams = nxLibGetCount(&error, path_to_cams);
	if (is_error(error)) return 0;

	/* Print information for all cameras in the tree */
	for (camera_counter = 0; camera_counter < num_cams; ++camera_counter) {
		sprintf(path, "%s/\\%d/Status/ValidIpAddress", path_to_cams, camera_counter);
		if (!nxLibGetBool(&error, path) || is_error(error)) {
			continue;
		}

		sprintf(path, "%s/\\%d/Status/Open", path_to_cams, camera_counter);
		if (nxLibGetBool(&error, path) || is_error(error)) {
			continue;
		}

		sprintf(path, "%s/\\%d/Status/Available", path_to_cams, camera_counter);
		if (!nxLibGetBool(&error, path) || is_error(error)) {
			continue;
		}

		sprintf(path, "%s/\\%d/Status/ValidProjectorFirmware", path_to_cams, camera_counter);
		if (!nxLibGetBool(&error, path) || is_error(error)) {
			continue;
		}

		sprintf(path, "%s/\\%d/Status/ValidCameraFirmware", path_to_cams, camera_counter);
		if (!nxLibGetBool(&error, path) || is_error(error)) {
			continue;
		}

		sprintf(path, "%s/\\%d/Type", path_to_cams, camera_counter);
		strcpy(cameratype, nxLibGetString(&error, path));
		for (type_counter = 0; type_counter < num_types && !is_error(error); ++type_counter) {
			if (strcmp(cameratype, valid_types[type_counter]) == 0) {
				sprintf(path, "%s/\\%d/SerialNumber", path_to_cams, camera_counter);
				strcpy(serial, nxLibGetString(&error, path));
				is_camera_found = !is_error(error);
				break;
			}
		}
		if (is_camera_found) {
			break;
		}
	}

	if (is_camera_found) {
		strcpy(out_serial, serial);
	}

	return is_camera_found;
}

MAYBE_UNUSED static int print_list_of_cameras()
{
	int error;
	int num_cams;
	char path_to_cams[64];
	char path[128];
	int n;

	/* Path to the cameras node */
	strcpy(path_to_cams, "/Cameras");

	/* Get number of connected cameras */
	num_cams = nxLibGetCount(&error, path_to_cams);
	exit_on_error(error);

	/* Print information for all cameras in the tree */
	printf("Serial No          Model            Type                Status\n");
	for (n = 0; n < num_cams; ++n) {
		sprintf(path, "%s/\\%d/Status", path_to_cams, n);
		nxLibGetType(&error, path);
		if (error == NxLibItemInexistent) {
			continue;
		}

		sprintf(path, "%s/\\%d/SerialNumber", path_to_cams, n);
		printf("%-19s", nxLibGetString(&error, path));
		sprintf(path, "%s/\\%d/ModelName", path_to_cams, n);
		printf("%-17s", nxLibGetString(&error, path));
		sprintf(path, "%s/\\%d/Type", path_to_cams, n);
		printf("%-20s", nxLibGetString(&error, path));

		char* status = 0;
		int with_error = 0;
		sprintf(path, "%s/\\%d/Status/ValidIpAddress", path_to_cams, n);
		if (status == 0 && !nxLibGetBool(&error, path) && !is_error(error)) {
			status = "Invalid IP";
		}

		sprintf(path, "%s/\\%d/Status/Open", path_to_cams, n);
		if (status == 0 && nxLibGetBool(&error, path) && !is_error(error)) {
			status = "Open";
		}
		with_error += error;

		sprintf(path, "%s/\\%d/Status/Available", path_to_cams, n);
		if (status == 0 && !nxLibGetBool(&error, path) && !is_error(error)) {
			status = "In Use";
		}
		with_error += error;

		sprintf(path, "%s/\\%d/Status/ValidProjectorFirmware", path_to_cams, n);
		if (status == 0 && !nxLibGetBool(&error, path) && !is_error(error)) {
			status = "Invalid Firmware";
		}
		with_error += error;

		sprintf(path, "%s/\\%d/Status/ValidCameraFirmware", path_to_cams, n);
		if (status == 0 && !nxLibGetBool(&error, path) && !is_error(error)) {
			status = "Invalid Firmware";
		}
		with_error += error;
		if (with_error) {
			status = "Error";
		} else if (status == 0) {
			status = "Available";
		}

		printf("%-12s\n", status);
	}

	return 0;
}