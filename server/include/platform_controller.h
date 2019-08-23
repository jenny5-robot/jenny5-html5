// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------
#ifndef platform_controller_H
#define platform_controller_H

#include "roboclaw_controller.h"

//------------------------------------------------------------------------------
#define CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR "CANNOT_CONNECT_TO_JENNY5_PLATFORM\n" 
#define CANNOT_CONNECT_TO_JENNY5_PLATFORM 1

#define Connected_to_platform_STR "Connected to platform\n"

#define INITIAL_SPEED_PLATFORM 1200
#define STEP_SPEED_PLATFORM 700

#define ROTATE_SPEED_FACTOR 1


//------------------------------------------------------------------------------
class t_platform_controller {
public:
	t_roboclaw_controller roboclaw_controller;

	t_platform_controller();
	~t_platform_controller();

	int connect(const char* port);
	bool is_connected(void);
	void disconnect(void);
	bool setup(char* error_string);

	void send_get_roboclaw_firmware_version(void);

	// positive if move forward
	bool move_left_motor(int16_t speed, uint32_t acceleration);
	// positive if move forward
	bool move_right_motor(int16_t speed, uint32_t acceleration);

	int rotate_left(uint16_t speed, uint32_t acceleration);
	int rotate_right(uint16_t speed, uint32_t acceleration);

	int stop_motors(void);
};
//------------------------------------------------------------------------------
extern t_platform_controller platform_controller;

#endif