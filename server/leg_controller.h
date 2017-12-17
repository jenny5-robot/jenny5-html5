// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#ifndef leg_controller_H
#define leg_controller_H

#define CANNOT_CONNECT_TO_JENNY5_LEG_STR "CANNOT_CONNECT_TO_JENNY5_LEG\n" 
#define CANNOT_CONNECT_TO_JENNY5_LEG 1

#define Connected_to_leg_STR "Connected to leg\n"

#include "roboclaw_controller.h"

class t_leg_controller {
public:
	t_roboclaw_controller roboclaw_controller;

	t_leg_controller();
	~t_leg_controller();

	int connect(int LEG_COM_PORT);
	bool is_connected(void);
	void disconnect(void);
	bool setup(char* error_string);

	void send_get_roboclaw_firmware_version(void);

	// positive if expand
	void move_bottom_motor(int16_t speed, uint32_t acceleration);
	// positive if expand
	void move_top_motor(int16_t speed, uint32_t acceleration);

	// positive if expand
	void expand_bottom(int16_t speed, uint32_t acceleration);
	// positive if expand
	void contract_bottom(int16_t speed, uint32_t acceleration);

	// positive if expand
	void expand_top(int16_t speed, uint32_t acceleration);
	// positive if expand
	void contract_top(int16_t speed, uint32_t acceleration);

	// positive if expand
	void expand_both(int16_t speed, uint32_t acceleration);
	// positive if expand
	void contract_both(int16_t speed, uint32_t acceleration);

	void stop_motors(void);
};

extern t_leg_controller leg_controller;

#endif