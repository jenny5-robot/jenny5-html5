// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#include "leg_controller.h"
#include "jenny5_defs.h"

t_leg_controller leg_controller;

//------------------------------------------------------------------------
t_leg_controller::t_leg_controller()
{

}
//------------------------------------------------------------------------
t_leg_controller::~t_leg_controller()
{
	stop_motors();
	roboclaw_controller.close_connection();
}
//------------------------------------------------------------------------
int t_leg_controller::connect(int PLATFORM_COM_PORT)
{
	if (!roboclaw_controller.connect(PLATFORM_COM_PORT - 1, 115200)) {
		return CANNOT_CONNECT_TO_JENNY5_LEG;
	}

	roboclaw_controller.set_M1_max_current_limit(500);// max 5 Amps
	roboclaw_controller.set_M2_max_current_limit(500);// max 5 Amps

	return E_OK;
}
//------------------------------------------------------------------------
bool t_leg_controller::is_connected(void)
{
	return roboclaw_controller.is_open();
}
//------------------------------------------------------------------------
void t_leg_controller::disconnect(void)
{
	roboclaw_controller.close_connection();
}
//------------------------------------------------------------------------
bool t_leg_controller::setup(char* /*error_string*/)
{
	return true;
}
//------------------------------------------------------------------------
void t_leg_controller::send_get_roboclaw_firmware_version(void)
{

}
//------------------------------------------------------------------------

// positive if expand
void t_leg_controller::move_bottom_motor(int16_t speed, uint32_t acceleration)
{
	roboclaw_controller.drive_M1_with_signed_duty_and_acceleration(-speed, acceleration);
}
//------------------------------------------------------------------------
// positive if expand
void t_leg_controller::move_top_motor(int16_t speed, uint32_t acceleration)
{
	roboclaw_controller.drive_M2_with_signed_duty_and_acceleration(-speed, acceleration);
}
//------------------------------------------------------------------------
// positive if expand
void t_leg_controller::expand_bottom(int16_t speed, uint32_t acceleration)
{
	move_bottom_motor(speed, acceleration);
}
//------------------------------------------------------------------------
// negative if contract
void t_leg_controller::contract_bottom(int16_t speed, uint32_t acceleration)
{
	move_bottom_motor(-speed, acceleration);
}
//------------------------------------------------------------------------
// positive if expand
void t_leg_controller::expand_top(int16_t speed, uint32_t acceleration)
{
	move_top_motor(speed, acceleration);
}
//------------------------------------------------------------------------
// positive if expand
void t_leg_controller::contract_top(int16_t speed, uint32_t acceleration)
{
	move_top_motor(-speed, acceleration);
}
//------------------------------------------------------------------------
// positive if expand
void t_leg_controller::expand_both(int16_t speed, uint32_t acceleration)
{
	expand_top(speed, acceleration);
	expand_bottom(speed, acceleration);
}
//------------------------------------------------------------------------
// positive if expand
void t_leg_controller::contract_both(int16_t speed, uint32_t acceleration)
{
	contract_top(speed, acceleration);
	contract_bottom(speed, acceleration);
}
//------------------------------------------------------------------------
void t_leg_controller::stop_motors(void)
{
	roboclaw_controller.drive_M1_with_signed_duty_and_acceleration(0, 1);
	roboclaw_controller.drive_M2_with_signed_duty_and_acceleration(0, 1);
}
//------------------------------------------------------------------------