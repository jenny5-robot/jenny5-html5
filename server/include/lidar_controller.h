// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#ifndef lidar_controller_H
#define lidar_controller_H

#include "jenny5_arduino_controller.h"

#define LIDAR_NUM_STEPS 200

#define CANNOT_CONNECT_TO_JENNY5_LIDAR_STR "CANNOT_CONNECT_TO_JENNY5_LIDAR\n" 
#define CANNOT_CONNECT_TO_JENNY5_LIDAR_ERROR 1

#define LIDAR_does_not_respond_STR "LIDAR does not respond! Game over!\n"
#define LIDAR_does_not_respond_ERROR 2

#define Connected_to_LIDAR_STR "Connected to LIDAR\n"

struct t_lidar_controller {
	t_jenny5_arduino_controller arduino_controller;
	int lidar_distances[LIDAR_NUM_STEPS];

	t_lidar_controller(void);
	void disconnect(void);
	bool is_connected(void);

	int connect(const char* port);
	bool setup(char* error_string);
	bool update_data(void);

	const char *error_to_string(int error);
};

extern t_lidar_controller LIDAR_controller;

#endif