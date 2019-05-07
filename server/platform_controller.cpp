// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#include "platform_controller.h"
#include "jenny5_defs.h"

t_platform_controller platform_controller;

//------------------------------------------------------------------------
t_platform_controller::t_platform_controller()
{

}
//------------------------------------------------------------------------
t_platform_controller::~t_platform_controller()
{
	stop_motors();
	roboclaw_controller.close_connection();
}
//------------------------------------------------------------------------
int t_platform_controller::connect(const char* port)
{
	if (!roboclaw_controller.connect(port, 115200)) {
		return CANNOT_CONNECT_TO_JENNY5_PLATFORM;
	}

	roboclaw_controller.set_M1_max_current_limit(800);// max 8 Amps
	roboclaw_controller.set_M2_max_current_limit(800);// max 8 Amps

	return E_OK;
}
//------------------------------------------------------------------------
bool t_platform_controller::is_connected(void)
{
	return roboclaw_controller.is_open();
}
//------------------------------------------------------------------------
void t_platform_controller::disconnect(void)
{
	roboclaw_controller.close_connection();
}
//------------------------------------------------------------------------
bool t_platform_controller::setup(char* /*error_string*/)
{
	return true;
}
//------------------------------------------------------------------------
void t_platform_controller::send_get_roboclaw_firmware_version(void)
{

}
//------------------------------------------------------------------------
bool t_platform_controller::move_left_motor(int16_t speed, uint32_t acceleration)
{
	return roboclaw_controller.drive_M2_with_signed_duty_and_acceleration(-speed, acceleration);
}
//------------------------------------------------------------------------
bool t_platform_controller::move_right_motor(int16_t speed, uint32_t acceleration)
{
	return roboclaw_controller.drive_M1_with_signed_duty_and_acceleration(-speed, acceleration);
}
//------------------------------------------------------------------------
int t_platform_controller::stop_motors(void)
{
	bool m1 = roboclaw_controller.drive_M2_with_signed_duty_and_acceleration(0, 1);
	bool m2 = roboclaw_controller.drive_M1_with_signed_duty_and_acceleration(0, 1);

	if (!m1)
		return 1;
	if (!m2)
		return 2;
	return 0;
}
//------------------------------------------------------------------------
int t_platform_controller::rotate_left(uint16_t speed, uint32_t /*acceleration*/)
{
	bool m1 = move_right_motor(speed, 1);
	bool m2 = move_left_motor(-speed, 1);

	if (!m1)
		return 1;
	if (!m2)
		return 2;
	return 0;
}
//------------------------------------------------------------------------
int t_platform_controller::rotate_right(uint16_t speed, uint32_t /*acceleration*/)
{
	bool m1 = move_right_motor(-speed, 1);
	bool m2 = move_left_motor(speed, 1);

	if (!m1)
		return 1;
	if (!m2)
		return 2;
	return 0;
}
//------------------------------------------------------------------------