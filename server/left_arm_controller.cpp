// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------
#include "left_arm_controller.h"
#include "jenny5_defs.h"

t_left_arm_controller left_arm_controller;
//----------------------------------------------------------------
t_left_arm_controller::t_left_arm_controller(void)
{

}
//----------------------------------------------------------------
bool t_left_arm_controller::is_connected(void)
{
	return arduino_controller.is_open();
}
//----------------------------------------------------------------
void t_left_arm_controller::disconnect(void)
{
	arduino_controller.close_connection();
}
//----------------------------------------------------------------
int t_left_arm_controller::connect(int LEFT_ARM_COM_PORT)
{
	//-------------- START INITIALIZATION ------------------------------

	if (!arduino_controller.connect(LEFT_ARM_COM_PORT - 1, 115200)) { // real - 1
		return CANNOT_CONNECT_TO_JENNY5_LEFT_ARM_ERROR;
	}

	bool left_arm_responded = false;

	// now wait to see if I have been connected
	// wait for no more than 3 seconds. If it takes more it means that something is not right, so we have to abandon it
	clock_t start_time = clock();

	while (1) {
		if (!arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (!left_arm_responded)
			if (arduino_controller.query_for_event(IS_ALIVE_EVENT, 0)) { // have we received the event from Serial ?
				left_arm_responded = true;
				break;
			}
		// measure the passed time 
		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 3 seconds then game over
		if (wait_time > NUM_SECONDS_TO_WAIT_FOR_CONNECTION) {

			if (!left_arm_responded)
				return LEFT_ARM_does_not_respond_ERROR;
		}
	}

	return E_OK;
}
//----------------------------------------------------------------
bool t_left_arm_controller::setup(char* error_string)
{

	int left_arm_motors_dir_pins[6] = { 12, 10, 8, 6, 4, 2 };
	int left_arm_motors_step_pins[6] = { 13, 11, 9, 7, 5, 3 };
	int left_arm_motors_enable_pins[6] = { 14, 14, 14, 14, 14, 14 };
	arduino_controller.send_create_stepper_motors(6, left_arm_motors_dir_pins, left_arm_motors_step_pins, left_arm_motors_enable_pins);


	int left_arm_potentiometer_pins[5] = { 2, 3, 4, 5, 6 };
	arduino_controller.send_create_potentiometers(5, left_arm_potentiometer_pins);

	int gripper_infrared_pins[1] = { 7 };

	arduino_controller.send_create_infrared_sensors(1, gripper_infrared_pins);

	int gripper_button_pins[1] = { LEFT_ARM_GRIPPER_BUTTON_PIN };

	arduino_controller.send_create_buttons(1, gripper_button_pins);

	clock_t start_time = clock();

	bool motors_controller_created = false;
	bool potentiometers_controller_created = false;
	bool infrared_controller_created = false;
	bool buttons_controller_created = false;

	while (1) {
		if (!arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (arduino_controller.query_for_event(STEPPER_MOTORS_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			motors_controller_created = true;

		if (arduino_controller.query_for_event(INFRARED_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			infrared_controller_created = true;

		if (arduino_controller.query_for_event(BUTTONS_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			buttons_controller_created = true;

		if (arduino_controller.query_for_event(POTENTIOMETERS_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			potentiometers_controller_created = true;

		if (motors_controller_created && potentiometers_controller_created && infrared_controller_created && buttons_controller_created)
			break;

		// measure the passed time 
		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 3 seconds then game over
		if (wait_time > NUM_SECONDS_TO_WAIT_FOR_CONNECTION) {
			if (!motors_controller_created)
				sprintf(error_string, "Cannot create left arm's motors controller! Game over!\n");
			if (!potentiometers_controller_created)
				sprintf(error_string, "Cannot create left arm's potentiometers controller! Game over!\n");
			if (!infrared_controller_created)
				sprintf(error_string, "Cannot create left arm's infrared controller! Game over!\n");
			if (!buttons_controller_created)
				sprintf(error_string, "Cannot create left arm's buttons controller! Game over!\n");
			return false;
		}
	}

	arduino_controller.send_set_stepper_motor_speed_and_acceleration(LEFT_ARM_GRIPPER_MOTOR, ARM_DEFAULT_MOTOR_SPEED, ARM_DEFAULT_MOTOR_ACCELERATION);

	arduino_controller.send_set_stepper_motor_speed_and_acceleration(LEFT_ARM_BODY_MOTOR, ARM_DEFAULT_MOTOR_SPEED, ARM_DEFAULT_MOTOR_ACCELERATION);

	arduino_controller.send_set_stepper_motor_speed_and_acceleration(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR, ARM_DEFAULT_MOTOR_SPEED, ARM_DEFAULT_MOTOR_ACCELERATION);

	arduino_controller.send_set_stepper_motor_speed_and_acceleration(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR, ARM_DEFAULT_MOTOR_SPEED, ARM_DEFAULT_MOTOR_ACCELERATION);

	arduino_controller.send_set_stepper_motor_speed_and_acceleration(LEFT_ARM_ELBOW_MOTOR, ARM_DEFAULT_MOTOR_SPEED, ARM_DEFAULT_MOTOR_ACCELERATION);

	arduino_controller.send_set_stepper_motor_speed_and_acceleration(LEFT_ARM_FOREARM_MOTOR, ARM_DEFAULT_MOTOR_SPEED, ARM_DEFAULT_MOTOR_ACCELERATION);

	//	Sleep(100);
	bool LEFT_ARM_BODY_MOTOR_set_speed_accell = false;
	bool LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_set_speed_accell = false;
	bool LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_set_speed_accell = false;
	bool LEFT_ARM_ELBOW_MOTOR_set_speed_accell = false;
	bool LEFT_ARM_FOREARM_MOTOR_set_speed_accell = false;
	bool LEFT_ARM_GRIPPER_MOTOR_set_speed_accell = false;

	start_time = clock();

	while (1) {
		if (!arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, LEFT_ARM_BODY_MOTOR))  // have we received the event from Serial ?
			LEFT_ARM_BODY_MOTOR_set_speed_accell = true;

		if (arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, LEFT_ARM_SHOULDER_UP_DOWN_MOTOR))  // have we received the event from Serial ?
			LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_set_speed_accell = true;

		if (arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR))  // have we received the event from Serial ?
			LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_set_speed_accell = true;

		if (arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, LEFT_ARM_ELBOW_MOTOR))  // have we received the event from Serial ?
			LEFT_ARM_ELBOW_MOTOR_set_speed_accell = true;

		if (arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, LEFT_ARM_FOREARM_MOTOR))  // have we received the event from Serial ?
			LEFT_ARM_FOREARM_MOTOR_set_speed_accell = true;

		if (arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, LEFT_ARM_GRIPPER_MOTOR))  // have we received the event from Serial ?
			LEFT_ARM_GRIPPER_MOTOR_set_speed_accell = true;


		if (LEFT_ARM_BODY_MOTOR_set_speed_accell && LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_set_speed_accell && LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_set_speed_accell &&
			LEFT_ARM_ELBOW_MOTOR_set_speed_accell && LEFT_ARM_FOREARM_MOTOR_set_speed_accell && LEFT_ARM_GRIPPER_MOTOR_set_speed_accell)
			break;

		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 3 seconds then game over
		if (wait_time > NUM_SECONDS_TO_WAIT_FOR_CONNECTION) {
			if (!LEFT_ARM_BODY_MOTOR_set_speed_accell)
				sprintf(error_string, "Cannot LEFT_ARM_BODY_MOTOR_set_speed_accell! Game over!\n");

			if (!LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_set_speed_accell)
				sprintf(error_string, "Cannot LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_set_speed_accell! Game over!\n");

			if (!LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_set_speed_accell)
				sprintf(error_string, "Cannot LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_set_speed_accell! Game over!\n");

			if (!LEFT_ARM_ELBOW_MOTOR_set_speed_accell)
				sprintf(error_string, "Cannot LEFT_ARM_ELBOW_MOTOR_set_speed_accell! Game over!\n");

			if (!LEFT_ARM_FOREARM_MOTOR_set_speed_accell)
				sprintf(error_string, "Cannot LEFT_ARM_FOREARM_MOTOR_set_speed_accell! Game over!\n");

			if (!LEFT_ARM_GRIPPER_MOTOR_set_speed_accell)
				sprintf(error_string, "Cannot LEFT_ARM_GRIPPER_MOTOR_set_speed_accell! Game over!\n");

			return false;
		}
	}

	int potentiometer_index_LEFT_ARM_BODY_MOTOR[1] = { 0 };
	int potentiometer_index_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR[1] = { 1 };
	int potentiometer_index_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR[1] = { 2 };
	int potentiometer_index_LEFT_ARM_ELBOW_MOTOR[1] = { 3 };
	int potentiometer_index_LEFT_ARM_FOREARM_MOTOR[1] = { 4 };
	int potentiometer_index_LEFT_ARM_GRIPPER_MOTOR[1] = { 5 };

	int potentiometer_min_LEFT_ARM_BODY_MOTOR[1] = { _potentiometer_min_LEFT_ARM_BODY_MOTOR };
	int potentiometer_max_LEFT_ARM_BODY_MOTOR[1] = { _potentiometer_max_LEFT_ARM_BODY_MOTOR };
	int potentiometer_home_LEFT_ARM_BODY_MOTOR[1] = { _potentiometer_home_LEFT_ARM_BODY_MOTOR };
	int potentiometer_dir_LEFT_ARM_BODY_MOTOR[1] = { -1 };

	int potentiometer_min_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR[1] = { _potentiometer_min_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR };
	int potentiometer_max_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR[1] = { _potentiometer_max_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR };
	int potentiometer_home_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR[1] = { _potentiometer_home_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR };
	int potentiometer_dir_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR[1] = { -1 };

	int potentiometer_min_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR[1] = { _potentiometer_min_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR };
	int potentiometer_max_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR[1] = { _potentiometer_max_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR };
	int potentiometer_home_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR[1] = { _potentiometer_home_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR };
	int potentiometer_dir_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR[1] = { 1 };

	int potentiometer_min_LEFT_ARM_ELBOW_MOTOR[1] = { _potentiometer_min_LEFT_ARM_ELBOW_MOTOR };
	int potentiometer_max_LEFT_ARM_ELBOW_MOTOR[1] = { _potentiometer_max_LEFT_ARM_ELBOW_MOTOR };
	int potentiometer_home_LEFT_ARM_ELBOW_MOTOR[1] = { 630 };
	int potentiometer_dir_LEFT_ARM_ELBOW_MOTOR[1] = { 1 };

	int potentiometer_min_LEFT_ARM_FOREARM_MOTOR[1] = { _potentiometer_min_LEFT_ARM_FOREARM_MOTOR };
	int potentiometer_max_LEFT_ARM_FOREARM_MOTOR[1] = { _potentiometer_max_LEFT_ARM_FOREARM_MOTOR };
	int potentiometer_home_LEFT_ARM_FOREARM_MOTOR[1] = { _potentiometer_home_LEFT_ARM_FOREARM_MOTOR };
	int potentiometer_dir_LEFT_ARM_FOREARM_MOTOR[1] = { 1 };

	int button_index_LEFT_ARM_GRIPPER_MOTOR[1] = { 0 };
	int button_direction_LEFT_ARM_GRIPPER_MOTOR[1] = { 1 };

	arduino_controller.send_attach_sensors_to_stepper_motor(LEFT_ARM_BODY_MOTOR, 1,
		potentiometer_index_LEFT_ARM_BODY_MOTOR,
		potentiometer_min_LEFT_ARM_BODY_MOTOR,
		potentiometer_max_LEFT_ARM_BODY_MOTOR,
		potentiometer_home_LEFT_ARM_BODY_MOTOR,
		potentiometer_dir_LEFT_ARM_BODY_MOTOR,
		0, NULL, 0, NULL, NULL);

	arduino_controller.send_attach_sensors_to_stepper_motor(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR, 1,
		potentiometer_index_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR,
		potentiometer_min_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR,
		potentiometer_max_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR,
		potentiometer_home_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR,
		potentiometer_dir_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR, 0, NULL, 0, NULL, NULL);

	arduino_controller.send_attach_sensors_to_stepper_motor(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR, 1,
		potentiometer_index_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR,
		potentiometer_min_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR,
		potentiometer_max_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR,
		potentiometer_home_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR,
		potentiometer_dir_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR, 0, NULL, 0, NULL, NULL);

	arduino_controller.send_attach_sensors_to_stepper_motor(LEFT_ARM_ELBOW_MOTOR, 1,
		potentiometer_index_LEFT_ARM_ELBOW_MOTOR,
		potentiometer_min_LEFT_ARM_ELBOW_MOTOR,
		potentiometer_max_LEFT_ARM_ELBOW_MOTOR,
		potentiometer_home_LEFT_ARM_ELBOW_MOTOR,
		potentiometer_dir_LEFT_ARM_ELBOW_MOTOR, 0, NULL, 0, NULL, NULL);

	arduino_controller.send_attach_sensors_to_stepper_motor(LEFT_ARM_FOREARM_MOTOR, 1,
		potentiometer_index_LEFT_ARM_FOREARM_MOTOR,
		potentiometer_min_LEFT_ARM_FOREARM_MOTOR,
		potentiometer_max_LEFT_ARM_FOREARM_MOTOR,
		potentiometer_home_LEFT_ARM_FOREARM_MOTOR,
		potentiometer_dir_LEFT_ARM_FOREARM_MOTOR, 0, NULL, 0, NULL, NULL);

	arduino_controller.send_attach_sensors_to_stepper_motor(LEFT_ARM_GRIPPER_MOTOR, 0,
		NULL, NULL, NULL, NULL, NULL, 0, NULL, 1, button_index_LEFT_ARM_GRIPPER_MOTOR, button_direction_LEFT_ARM_GRIPPER_MOTOR);

	strcpy(error_string, "LEFT ARM SETUP PERFECT\n");
	return true;
}
//----------------------------------------------------------------
bool t_left_arm_controller::home_all_motors(char* error_string)
{
	// must home the left arm
	// except gripper until a new one is printed

	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_BODY_MOTOR);

	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR);

	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);

	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_ELBOW_MOTOR);

	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_FOREARM_MOTOR);

	//arduino_controller.send_go_home_stepper_motor(LEFT_ARM_GRIPPER_MOTOR);

	printf("Left arm motors home started ...\n");
	clock_t start_time = clock();
	bool motor_homed_LEFT_ARM_BODY_MOTOR = false;
	bool motor_homed_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR = false;
	bool motor_homed_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR = false;
	bool motor_homed_LEFT_ARM_ELBOW_MOTOR = false;
	bool motor_homed_LEFT_ARM_FOREARM_MOTOR = false;
//	bool motor_homed_LEFT_ARM_GRIPPER_MOTOR = false;

	while (1) {
		if (!arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (!motor_homed_LEFT_ARM_BODY_MOTOR)
			if (arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, LEFT_ARM_BODY_MOTOR))  // have we received the event from Serial ?
				motor_homed_LEFT_ARM_BODY_MOTOR = true;

		if (!motor_homed_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR)
			if (arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, LEFT_ARM_SHOULDER_UP_DOWN_MOTOR))  // have we received the event from Serial ?
				motor_homed_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR = true;

		if (!motor_homed_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR)
			if (arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR))  // have we received the event from Serial ?
				motor_homed_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR = true;

		if (!motor_homed_LEFT_ARM_ELBOW_MOTOR)
			if (arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, LEFT_ARM_ELBOW_MOTOR))  // have we received the event from Serial ?
				motor_homed_LEFT_ARM_ELBOW_MOTOR = true;

		if (!motor_homed_LEFT_ARM_FOREARM_MOTOR)
			if (arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, LEFT_ARM_FOREARM_MOTOR))  // have we received the event from Serial ?
				motor_homed_LEFT_ARM_FOREARM_MOTOR = true;
/*
		if (!motor_homed_LEFT_ARM_GRIPPER_MOTOR)
			if (arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, LEFT_ARM_GRIPPER_MOTOR))  // have we received the event from Serial ?
				motor_homed_LEFT_ARM_GRIPPER_MOTOR = true;
				*/
		if (motor_homed_LEFT_ARM_BODY_MOTOR && motor_homed_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR && motor_homed_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR &&
			motor_homed_LEFT_ARM_ELBOW_MOTOR && motor_homed_LEFT_ARM_FOREARM_MOTOR)// && motor_homed_LEFT_ARM_GRIPPER_MOTOR)
			break;

		// measure the passed time 
		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 5 seconds and no home
		if (wait_time > 5) {
			if (!motor_homed_LEFT_ARM_BODY_MOTOR)
				sprintf(error_string, "Cannot home motor_homed_LEFT_ARM_BODY_MOTOR! Game over!\n");

			if (!motor_homed_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR)
				sprintf(error_string, "Cannot home motor_homed_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR! Game over!\n");

			if (!motor_homed_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR)
				sprintf(error_string, "Cannot home motor_homed_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR! Game over!\n");

			if (!motor_homed_LEFT_ARM_ELBOW_MOTOR)
				sprintf(error_string, "Cannot home motor_homed_LEFT_ARM_ELBOW_MOTOR! Game over!\n");

			if (!motor_homed_LEFT_ARM_FOREARM_MOTOR)
				sprintf(error_string, "Cannot home motor_homed_LEFT_ARM_FOREARM_MOTOR! Game over!\n");

			//if (!motor_homed_LEFT_ARM_GRIPPER_MOTOR)
				//sprintf(error_string, "Cannot home motor_homed_LEFT_ARM_GRIPPER_MOTOR! Game over!\n");
			return false;
		}
	}
	return true;
}
//----------------------------------------------------------------
void t_left_arm_controller::send_get_arduino_firmware_version(void)
{

}
//----------------------------------------------------------------
void t_left_arm_controller::send_get_sensors_value(void)
{
	arduino_controller.send_get_potentiometer_position(LEFT_ARM_BODY_POTENTIOMETER_INDEX);
	arduino_controller.send_get_potentiometer_position(LEFT_ARM_SHOULDER_UP_DOWN_POTENTIOMETER_INDEX);
	arduino_controller.send_get_potentiometer_position(LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX);
	arduino_controller.send_get_potentiometer_position(LEFT_ARM_ELBOW_POTENTIOMETER_INDEX);
	arduino_controller.send_get_potentiometer_position(LEFT_ARM_FOREARM_POTENTIOMETER_INDEX);
	arduino_controller.send_get_infrared_signal_strength(LEFT_ARM_GRIPPER_INFRARED_INDEX);

	arduino_controller.send_get_button_state(LEFT_ARM_GRIPPER_BUTTON_INDEX);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_disable_motors(void)
{
	arduino_controller.send_disable_stepper_motor(LEFT_ARM_BODY_MOTOR);// this will disable all
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_BODY_MOTOR_home(void)
{
	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_BODY_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_home(void)
{
	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_home(void)
{
	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_ELBOW_MOTOR_home(void)
{
	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_ELBOW_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_FOREARM_MOTOR_home(void)
{
	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_FOREARM_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_GRIPPER_MOTOR_home(void)
{
	arduino_controller.send_go_home_stepper_motor(LEFT_ARM_GRIPPER_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_BODY_MOTOR_to_sensor_position(int new_position)
{
	arduino_controller.send_stepper_motor_goto_sensor_position(LEFT_ARM_BODY_MOTOR, new_position);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_to_sensor_position(int new_position)
{
	arduino_controller.send_stepper_motor_goto_sensor_position(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR, new_position);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(int new_position)
{
	arduino_controller.send_stepper_motor_goto_sensor_position(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR, new_position);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_ELBOW_MOTOR_to_sensor_position(int new_position)
{
	arduino_controller.send_stepper_motor_goto_sensor_position(LEFT_ARM_ELBOW_MOTOR, new_position);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_FOREARM_MOTOR_to_sensor_position(int new_position)
{
	arduino_controller.send_stepper_motor_goto_sensor_position(LEFT_ARM_FOREARM_MOTOR, new_position);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_GRIPPER_MOTOR_start_open(void)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_GRIPPER_MOTOR, -5000);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_GRIPPER_MOTOR_stop_open(void)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_GRIPPER_MOTOR, 0);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_BODY_MOTOR_move(int num_steps, int speed, int accelleration)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_BODY_MOTOR, num_steps);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_move(int num_steps, int speed, int accelleration)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR, num_steps);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(int num_steps, int speed, int accelleration)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR, num_steps);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_ELBOW_MOTOR_move(int num_steps, int speed, int accelleration)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_ELBOW_MOTOR, num_steps);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_FOREARM_MOTOR_move(int num_steps, int speed, int accelleration)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_FOREARM_MOTOR, num_steps);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_LEFT_ARM_GRIPPER_MOTOR_move(int num_steps, int speed, int accelleration)
{
	arduino_controller.send_move_stepper_motor(LEFT_ARM_GRIPPER_MOTOR, num_steps);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_stop_motors(void)
{
	arduino_controller.send_stop_stepper_motor(LEFT_ARM_BODY_MOTOR);
	arduino_controller.send_stop_stepper_motor(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR);
	arduino_controller.send_stop_stepper_motor(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);
	arduino_controller.send_stop_stepper_motor(LEFT_ARM_ELBOW_MOTOR);
	arduino_controller.send_stop_stepper_motor(LEFT_ARM_FOREARM_MOTOR);
	arduino_controller.send_stop_stepper_motor(LEFT_ARM_GRIPPER_MOTOR);
}
//----------------------------------------------------------------
void t_left_arm_controller::send_stop_motor(int motor_index)
{
	arduino_controller.send_stop_stepper_motor(motor_index);
}
//----------------------------------------------------------------
char *t_left_arm_controller::error_to_string(int error)
{
	switch (error) {
	case E_OK:
		return "LEFT ARM PERFECT\n";
	case CANNOT_CONNECT_TO_JENNY5_LEFT_ARM_ERROR:
		return CANNOT_CONNECT_TO_JENNY5_LEFT_ARM_STR;
	case LEFT_ARM_does_not_respond_ERROR:
		return LEFT_ARM_does_not_respond_STR;
	}
	return NULL;
}
//----------------------------------------------------------------
bool t_left_arm_controller::wave_hand(char *error_string)
{
	if (!home_all_motors(error_string))
		return false;

	jenny5_event motor_move_done_event(STEPPER_MOTOR_MOVE_DONE_EVENT);

	motor_move_done_event.param1 = LEFT_ARM_ELBOW_POTENTIOMETER_INDEX;
	send_LEFT_ARM_ELBOW_MOTOR_to_sensor_position(900);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_UP_DOWN_POTENTIOMETER_INDEX;
	send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_to_sensor_position(550);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX;
	send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(500);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX;
	send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(_potentiometer_min_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX;
	send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(500);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX;
	send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(_potentiometer_min_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX;
	motor_move_done_event.param1 = LEFT_ARM_SHOULDER_UP_DOWN_POTENTIOMETER_INDEX;
	send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_to_sensor_position(_potentiometer_home_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;

	motor_move_done_event.param1 = LEFT_ARM_ELBOW_POTENTIOMETER_INDEX;
	send_LEFT_ARM_ELBOW_MOTOR_to_sensor_position(_potentiometer_home_LEFT_ARM_ELBOW_MOTOR);
	if (!arduino_controller.wait_for_command_completion(motor_move_done_event, EVENT_INFO_TYPE | EVENT_INFO_PARAM1))
		return false;


	return true;
}
//----------------------------------------------------------------