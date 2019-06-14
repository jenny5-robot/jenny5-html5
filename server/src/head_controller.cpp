// author: Mihai Oltean
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#include "head_controller.h"
#include "jenny5_defs.h"

using namespace cv;

t_head_controller jenny5_head_controller;

//--------------------------------------------------------
t_head_controller::t_head_controller(void)
{

}
//--------------------------------------------------------
int t_head_controller::connect(const char* port)
{
	//-------------- START INITIALIZATION ------------------------------

	if (!head_arduino_controller.connect(port, 115200)) { 
		return CANNOT_CONNECT_TO_JENNY5_HEAD_ERROR;
	}

	bool head_responded = false;

	// now wait to see if I have been connected
	// wait for no more than 3 seconds. If it takes more it means that something is not right, so we have to abandon it
	clock_t start_time = clock();

	while (1) {
		if (!head_arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (!head_responded)
			if (head_arduino_controller.query_for_event(IS_ALIVE_EVENT, 0)) { // have we received the event from Serial ?
				head_responded = true;
				break;
			}

		// measure the passed time 
		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 3 seconds then game over
		if (wait_time > NUM_SECONDS_TO_WAIT_FOR_CONNECTION) {
			if (!head_responded)
			  return Head_does_not_respond_ERROR;
		}
	}

	return E_OK;
}
//----------------------------------------------------------------
void t_head_controller::disconnect(void)
{
	head_arduino_controller.close_connection();
}
//----------------------------------------------------------------
bool t_head_controller::is_connected(void)
{
	return head_arduino_controller.is_open();
}
//----------------------------------------------------------------
bool t_head_controller::setup(char* error_string)
{
	int head_motors_dir_pins[2] = { 5, 2 };
	int head_motors_step_pins[2] = { 6, 3 };
	int head_motors_enable_pins[2] = { 7, 4 };
	head_arduino_controller.send_create_stepper_motors(2, head_motors_dir_pins, head_motors_step_pins, head_motors_enable_pins);

	int head_sonars_trig_pins[1] = { 10 };
	int head_sonars_echo_pins[1] = { 11 };

	head_arduino_controller.send_create_ultrasonics_HC_SR04(1, head_sonars_trig_pins, head_sonars_echo_pins);

	int head_potentiometer_pins[2] = { 0, 1 };
	head_arduino_controller.send_create_potentiometers(2, head_potentiometer_pins);

	bool motors_controller_created = false;
	bool sonars_controller_created = false;
	bool potentiometers_controller_created = false;

	clock_t start_time = clock();
	while (1) {
		if (!head_arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (head_arduino_controller.query_for_event(STEPPER_MOTORS_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			motors_controller_created = true;

		if (head_arduino_controller.query_for_event(ULTRASONICS_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			sonars_controller_created = true;

		if (head_arduino_controller.query_for_event(POTENTIOMETERS_CONTROLLER_CREATED_EVENT, 0))  // have we received the event from Serial ?
			potentiometers_controller_created = true;

		if (motors_controller_created && sonars_controller_created && potentiometers_controller_created)
			break;

		// measure the passed time 
		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 3 seconds then game over
		if (wait_time > NUM_SECONDS_TO_WAIT_FOR_CONNECTION) {
			if (!motors_controller_created)
				sprintf(error_string, "Cannot create head's motors controller! Game over!\n");
			if (!sonars_controller_created)
				sprintf(error_string, "Cannot create head's sonars controller! Game over!\n");
			if (!potentiometers_controller_created)
				sprintf(error_string, "Cannot create head's potentiometers controller! Game over!\n");
			return false;
		}
	}

	head_arduino_controller.send_set_stepper_motor_speed_and_acceleration(HEAD_MOTOR_NECK, 1500, 500);
	head_arduino_controller.send_set_stepper_motor_speed_and_acceleration(HEAD_MOTOR_FACE, 1500, 500);

	bool HEAD_MOTOR_HORIZONTAL_set_speed_accell = false;
	bool HEAD_MOTOR_VERTICAL_set_speed_accell = false;

	start_time = clock();

	while (1) {
		if (!head_arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (head_arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, HEAD_MOTOR_NECK))  // have we received the event from Serial ?
			HEAD_MOTOR_HORIZONTAL_set_speed_accell = true;
		if (head_arduino_controller.query_for_event(STEPPER_MOTOR_SET_SPEED_ACCELL_EVENT, HEAD_MOTOR_FACE))  // have we received the event from Serial ?
			HEAD_MOTOR_VERTICAL_set_speed_accell = true;

		if (HEAD_MOTOR_HORIZONTAL_set_speed_accell && HEAD_MOTOR_VERTICAL_set_speed_accell)
			break;

		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 3 seconds then game over
		if (wait_time > NUM_SECONDS_TO_WAIT_FOR_CONNECTION) {
			if (!HEAD_MOTOR_HORIZONTAL_set_speed_accell)
				sprintf(error_string, "Cannot HEAD_MOTOR_HORIZONTAL_set_speed_accell! Game over!\n");
			if (!HEAD_MOTOR_VERTICAL_set_speed_accell)
				sprintf(error_string, "Cannot HEAD_MOTOR_VERTICAL_set_speed_accell! Game over!\n");
			return false;
		}
	}
/*
	int potentiometer_index_head_horizontal_motor[1] = { 0 };
	int potentiometer_index_head_vertical_motor[1] = { 1 };


	int head_horizontal_motor_potentiometer_min[1] = { _head_horizontal_motor_potentiometer_min };
	int head_horizontal_motor_potentiometer_max[1] = { _head_horizontal_motor_potentiometer_max };
	int head_horizontal_motor_potentiometer_home[1] = { _head_horizontal_motor_potentiometer_home };
	int head_horizontal_motor_potentiometer_dir[1] = { -1 };


	int head_vertical_motor_potentiometer_min[1] = { _head_vertical_motor_potentiometer_min };
	int head_vertical_motor_potentiometer_max[1] = { _head_vertical_motor_potentiometer_max };
	int head_vertical_motor_potentiometer_home[1] = { _head_vertical_motor_potentiometer_home };
	int head_vertical_motor_potentiometer_dir[1] = { 1 };

	head_arduino_controller.send_attach_sensors_to_stepper_motor(HEAD_MOTOR_NECK,
		1, potentiometer_index_head_horizontal_motor, head_horizontal_motor_potentiometer_min, 
		head_horizontal_motor_potentiometer_max, head_horizontal_motor_potentiometer_home, 
		head_horizontal_motor_potentiometer_dir,
		0, NULL, 0, NULL, NULL);

	head_arduino_controller.send_attach_sensors_to_stepper_motor(HEAD_MOTOR_FACE, 1, potentiometer_index_head_vertical_motor, head_vertical_motor_potentiometer_min, 
		head_vertical_motor_potentiometer_max, head_vertical_motor_potentiometer_home, head_vertical_motor_potentiometer_dir,
		0, NULL, 0, NULL, NULL);
		*/
	strcpy(error_string, "HEAD SETUP PERFECT\n");
	return true;
}
//----------------------------------------------------------------
void t_head_controller::send_get_sensors_value(void)
{
	head_arduino_controller.send_get_potentiometer_position(HEAD_POTENTIOMETER_NECK_INDEX);
	head_arduino_controller.send_get_potentiometer_position(HEAD_POTENTIOMETER_FACE_INDEX);
	head_arduino_controller.send_get_ultrasonic_HC_SR04_distance(HEAD_ULTRASONIC_FACE_INDEX);
}
//--------------------------------------------------------------------------
void t_head_controller::send_home_all(void)
{
	head_arduino_controller.send_go_home_stepper_motor(HEAD_MOTOR_NECK);
	head_arduino_controller.send_go_home_stepper_motor(HEAD_MOTOR_FACE);
}
//------------------------------------------------------------------------
void t_head_controller::send_neck_home(void)
{
	head_arduino_controller.send_go_home_stepper_motor(HEAD_MOTOR_NECK);
}
//------------------------------------------------------------------------
void t_head_controller::send_face_home(void)
{
	head_arduino_controller.send_go_home_stepper_motor(HEAD_MOTOR_FACE);
}
//------------------------------------------------------------------------
void t_head_controller::send_get_arduino_firmware_version(void)
{
	head_arduino_controller.send_get_firmware_version();
}
//------------------------------------------------------------------------
void t_head_controller::send_neck_to_sensor_position(int head_neck_new_position)
{
	head_arduino_controller.send_stepper_motor_goto_sensor_position(HEAD_MOTOR_NECK, head_neck_new_position);
}
//------------------------------------------------------------------------
void t_head_controller::send_face_to_sensor_position(int head_face_new_position)
{
	head_arduino_controller.send_stepper_motor_goto_sensor_position(HEAD_MOTOR_FACE, head_face_new_position);
}
//------------------------------------------------------------------------
bool t_head_controller::home_all_motors(char* error_string)
{
	// must home the head
	head_arduino_controller.send_go_home_stepper_motor(HEAD_MOTOR_NECK);
	head_arduino_controller.send_go_home_stepper_motor(HEAD_MOTOR_FACE);

	sprintf(error_string, "Head motors home started ...\n");
	clock_t start_time = clock();
	bool horizontal_motor_homed = false;
	bool vertical_motor_homed = false;

	while (1) {
		if (!head_arduino_controller.update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		if (!horizontal_motor_homed)
			if (head_arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, HEAD_MOTOR_NECK))  // have we received the event from Serial ?
				horizontal_motor_homed = true;

		if (!vertical_motor_homed)
			if (head_arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, HEAD_MOTOR_FACE))  // have we received the event from Serial ?
				vertical_motor_homed = true;

		if (horizontal_motor_homed && vertical_motor_homed)
			break;

		// measure the passed time 
		clock_t end_time = clock();

		double wait_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		// if more than 5 seconds and no home
		if (wait_time > 5) {
			if (!vertical_motor_homed)
				sprintf(error_string, "Cannot home vertical motor! Game over!");
			if (!horizontal_motor_homed)
				sprintf(error_string, "Cannot home horizontal motor! Game over!");
			return false;
		}
	}

	sprintf(error_string, "Home motors DONE\n");
	return true;
}
//----------------------------------------------------------------
void t_head_controller::send_disable_motors(void)
{
	head_arduino_controller.send_disable_stepper_motor(HEAD_MOTOR_NECK);
	head_arduino_controller.send_disable_stepper_motor(HEAD_MOTOR_FACE);
}
//----------------------------------------------------------------
const char *t_head_controller::error_to_string(int error)
{
	switch (error) {
	case E_OK:
		return "HEAD PERFECT\n";
	case CANNOT_CONNECT_TO_JENNY5_HEAD_ERROR:
		return CANNOT_CONNECT_TO_JENNY5_HEAD_STR;
	case Head_does_not_respond_ERROR:
		return Head_does_not_respond_STR;
	}
	return NULL;
}
//----------------------------------------------------------------
void t_head_controller::send_HEAD_MOTOR_NECK_move(int num_steps/*, int speed, int accelleration*/)
{
	head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_NECK, num_steps);
}
//----------------------------------------------------------------
void t_head_controller::send_HEAD_MOTOR_FACE_move(int num_steps/*, int speed, int accelleration*/)
{
	head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, num_steps);
}
//----------------------------------------------------------------
void t_head_controller::send_stop_motors(void)
{
	head_arduino_controller.send_stop_stepper_motor(HEAD_MOTOR_NECK);
	head_arduino_controller.send_stop_stepper_motor(HEAD_MOTOR_FACE);
}
//----------------------------------------------------------------
void t_head_controller::send_stop_motor(int motor_index)
{
	head_arduino_controller.send_stop_stepper_motor(motor_index);
}
//----------------------------------------------------------------
bool t_head_controller::open_camera(void)
{
	if (!cam.isOpened()) {
		return cam.open(HEAD_CAMERA_INDEX);
	}
	else
		return true;
}
//----------------------------------------------------------------
void t_head_controller::release_camera(void)
{
	if (cam.isOpened())
		cam.release();
}
//----------------------------------------------------------------
int t_head_controller::capture_camera_and_save_to_disk(const char *file_name)
{
	if (!cam.isOpened())
		if (!cam.open(HEAD_CAMERA_INDEX))
			return CANNOT_CONNECT_TO_HEAD_CAMERA_ERROR;
		else {
			cam.set(CAP_PROP_FRAME_WIDTH, 352);
			cam.set(CAP_PROP_FRAME_HEIGHT, 240);
		}

	cv::Mat image;
	cam >> image;
	if (!cv::imwrite(file_name, image))
		return CANNOT_WRITE_HEAD_CAMERA_IMAGE_TO_DISK_ERROR;

	return E_OK;
}
//----------------------------------------------------------------
int t_head_controller::capture_camera_save_to_disk_and_release(const char *file_name)
{
	if (!cam.isOpened()) 
		if (!cam.open(HEAD_CAMERA_INDEX))
			return CANNOT_CONNECT_TO_HEAD_CAMERA_ERROR;
		else {
			cam.set(CAP_PROP_FRAME_WIDTH, 352);
			cam.set(CAP_PROP_FRAME_HEIGHT, 240);
		}

	cv::Mat image;
	cam >> image;
	if (!image.rows)
		return  CANNOT_CAPTURE_IMAGE_FROM_HEAD_CAMERA;
	if (!cv::imwrite(file_name, image))
		return CANNOT_WRITE_HEAD_CAMERA_IMAGE_TO_DISK_ERROR;
	
	cam.release();

	return E_OK;
}
//----------------------------------------------------------------