// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------

#include <thread>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "process_command.h"

#include "platform_controller.h"
#include "leg_controller.h"
#include "jenny5_defs.h"
#include "left_arm_controller.h"
#include "right_arm_controller.h"
#include "head_controller.h"
#include "setup_functions.h"
#include "head_face_follow.h"
#include "lidar_controller.h"
#include "follow_person.h"
#include "settings.h"
#include "jenny5_ports.h"

//--------------------------------------------------------

cv::CascadeClassifier face_classifier;
std::thread *face_tracking_thread;
std::thread *follow_person_thread;


int robot_stopped;
int disconnected;
int move_mode;

FILE *f_log;

//-----------------------------------------------------------------------
void init_robot(void)
{
	disconnected = false;
	move_mode = PAUSED_STATE;
}
//-----------------------------------------------------------------------
int is_disconected(void)
{
	return disconnected;
}
//-----------------------------------------------------------------------
void update_commands_from_serial(void)
{
	left_arm_controller.arduino_controller.update_commands_from_serial();
	left_arm_controller.arduino_controller.clear_events_list();

	right_arm_controller.arduino_controller.update_commands_from_serial();
	right_arm_controller.arduino_controller.clear_events_list();

	jenny5_head_controller.head_arduino_controller.update_commands_from_serial();
	jenny5_head_controller.head_arduino_controller.clear_events_list();
}
//-----------------------------------------------------------------------
int handle_navigate(char bx, char by)
{
	bool m1, m2;

	if (by > 32) {// move backward
		if (bx < 32) {// turn right
			m2 = platform_controller.move_right_motor(-INITIAL_SPEED_PLATFORM - (by - 32) * STEP_SPEED_PLATFORM - (32 - bx) * STEP_SPEED_PLATFORM * ROTATE_SPEED_FACTOR, 1);
			m1 = platform_controller.move_left_motor(-INITIAL_SPEED_PLATFORM - (by - 32) * STEP_SPEED_PLATFORM, 1);
		}
		else
			if (bx > 32) {// turn left
				m2 = platform_controller.move_right_motor(-INITIAL_SPEED_PLATFORM - (by - 32) * STEP_SPEED_PLATFORM, 1);
				m1 = platform_controller.move_left_motor(-INITIAL_SPEED_PLATFORM - (by - 32) * STEP_SPEED_PLATFORM - (bx - 32) * STEP_SPEED_PLATFORM * ROTATE_SPEED_FACTOR, 1);
			}
			else { // bx == 32
				//m1 = platform_controller.move_left_motor(0, 1);
				//m2 = platform_controller.move_right_motor(0, 1);
				m2 = platform_controller.move_right_motor(-INITIAL_SPEED_PLATFORM - (by - 32) * STEP_SPEED_PLATFORM, 1);
				m1 = platform_controller.move_left_motor(-INITIAL_SPEED_PLATFORM - (by - 32) * STEP_SPEED_PLATFORM, 1);

			}
	}
	else
		if (by < 32) { // move forward
			if (bx > 32) {// turn right
				m2 = platform_controller.move_left_motor(INITIAL_SPEED_PLATFORM + (32 - by) * STEP_SPEED_PLATFORM + (bx - 32) * STEP_SPEED_PLATFORM * ROTATE_SPEED_FACTOR, 1);
				m1 = platform_controller.move_right_motor(INITIAL_SPEED_PLATFORM + (32 - by) * STEP_SPEED_PLATFORM, 1);
			}
			else
				if (bx < 32) {// turn left
					m2 = platform_controller.move_left_motor(INITIAL_SPEED_PLATFORM + (32 - by) * STEP_SPEED_PLATFORM, 1);
					m1 = platform_controller.move_right_motor(INITIAL_SPEED_PLATFORM + (32 - by) * STEP_SPEED_PLATFORM + (32 - bx) * STEP_SPEED_PLATFORM * ROTATE_SPEED_FACTOR, 1);
				}
				else {// bx == 32
					m2 = platform_controller.move_left_motor(INITIAL_SPEED_PLATFORM + (32 - by) * STEP_SPEED_PLATFORM, 1);
					m1 = platform_controller.move_right_motor(INITIAL_SPEED_PLATFORM + (32 - by) * STEP_SPEED_PLATFORM, 1);
				}
		}
		else {
			m1 = platform_controller.move_left_motor(0, 1);
			m2 = platform_controller.move_right_motor(0, 1);
		}

		if (!m1)
			return 1;
		if (!m2)
			return 2;
		return 0;
}
//--------------------------------------------------------------------
int handle_platform_rotate(char bx, char /*by*/)
{
	bool m1, m2;

	if (bx < 32) // rotate right
		return platform_controller.rotate_left(INITIAL_SPEED_PLATFORM + (32 - bx) * STEP_SPEED_PLATFORM, 1);
	else
		if (bx > 32) // rotate left
			return platform_controller.rotate_right(INITIAL_SPEED_PLATFORM + (bx - 32) * STEP_SPEED_PLATFORM, 1);
		else {
			m1 = platform_controller.move_left_motor(0, 1);// stop
			m2 = platform_controller.move_right_motor(0, 1);

			if (!m1)
				return 1;
			if (!m2)
				return 2;
			return 0;
		}
}
//--------------------------------------------------------------------
void handle_leg_move(char /*bx*/, char by)
{
	char dead_zone_length = 6;
	if (by < 32 - dead_zone_length / 2) // contract
		leg_controller.contract_both((32 - by) * STEP_SPEED_LEG, 1);
	else
		if (by > 32 + dead_zone_length / 2) // expand
			leg_controller.expand_both((by - 32) * STEP_SPEED_LEG, 1);
}
//--------------------------------------------------------------------
void handle_left_arm_body_left_right_move(char bx, char /*by*/)
{
	// body left-right
	if (bx < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_ARM_BODY_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 500*/);
	else
		if (bx > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_ARM_BODY_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			left_arm_controller.send_stop_motor(LEFT_ARM_BODY_MOTOR);
}
//--------------------------------------------------------------------
void handle_left_arm_up_down_move(char /*bx*/, char by)
{
	// shoulder up-down
	if (by < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_ARM_SHOULDER_UP_DOWN_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 500*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_ARM_SHOULDER_UP_DOWN_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			left_arm_controller.send_stop_motor(LEFT_ARM_SHOULDER_UP_DOWN_MOTOR);
}
//--------------------------------------------------------------------
void handle_left_arm_rotate_move(char bx, char /*by*/)
{
	// shoulder left-right
	if (bx < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 500*/);
	else
		if (bx > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			left_arm_controller.send_stop_motor(LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);
}
//--------------------------------------------------------------------
void handle_left_arm_elbow_move(char /*bx*/, char by)
{
	// elbow up-down
	if (by < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_ARM_ELBOW_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 500*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_ARM_ELBOW_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			left_arm_controller.send_stop_motor(LEFT_ARM_ELBOW_MOTOR);
}
//--------------------------------------------------------------------
void handle_left_arm_forearm_move(char bx, char /*by*/)
{
	// forearm left-right
	if (bx < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_ARM_FOREARM_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 500*/);
	else
		if (bx > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_ARM_FOREARM_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - sto
			left_arm_controller.send_stop_motor(LEFT_ARM_FOREARM_MOTOR);
}
//--------------------------------------------------------------------
void handle_left_arm_wrist_move(char /*bx*/, char by)
{
	// wrist
	if (by < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_ARM_WRIST_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 500*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_ARM_WRIST_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			left_arm_controller.send_stop_motor(LEFT_ARM_WRIST_MOTOR);
}
//--------------------------------------------------------------------
void handle_left_arm_gripper_move(char /*bx*/, char /*by*/)
{
	/*
	// gripper
	if (by < 32 - ARM_dead_zone_length / 2) // 
		left_arm_controller.send_LEFT_ARM_WRIST_MOTOR_move(ARM_MOTOR_NUM_STEPS, (32 - by) * STEP_SPEED_ARM, 500);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			left_arm_controller.send_LEFT_ARM_GRIPPER_MOTOR_move(-ARM_MOTOR_NUM_STEPS, (by - 32) * STEP_SPEED_ARM, 500);
		else// inside dead zone - stop
			left_arm_controller.send_stop_motor(LEFT_ARM_GRIPPER_MOTOR);
			*/
}
//--------------------------------------------------------------------

void handle_right_arm_body_left_right_move(char bx, char /*by*/)
{
	// body left-right
	if (bx < 32 - ARM_dead_zone_length / 2) // 
		right_arm_controller.send_ARM_BODY_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 500*/);
	else
		if (bx > 32 + ARM_dead_zone_length / 2) // 
			right_arm_controller.send_ARM_BODY_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			right_arm_controller.send_stop_motor(RIGHT_ARM_BODY_MOTOR);
}
//--------------------------------------------------------------------
void handle_right_arm_up_down_move(char /*bx*/, char by)
{
	// shoulder up-down
	if (by < 32 - ARM_dead_zone_length / 2) // 
		right_arm_controller.send_ARM_SHOULDER_UP_DOWN_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 500*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			right_arm_controller.send_ARM_SHOULDER_UP_DOWN_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			right_arm_controller.send_stop_motor(RIGHT_ARM_SHOULDER_UP_DOWN_MOTOR);
}
//--------------------------------------------------------------------
void handle_right_arm_rotate_move(char bx, char /*by*/)
{
	// shoulder left-right
	if (bx < 32 - ARM_dead_zone_length / 2) // 
		right_arm_controller.send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 500*/);
	else
		if (bx > 32 + ARM_dead_zone_length / 2) // 
			right_arm_controller.send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			right_arm_controller.send_stop_motor(RIGHT_ARM_SHOULDER_LEFT_RIGHT_MOTOR);
}
//--------------------------------------------------------------------
void handle_right_arm_elbow_move(char /*bx*/, char by)
{
	// elbow up-down
	if (by < 32 - ARM_dead_zone_length / 2) // 
		right_arm_controller.send_ARM_ELBOW_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 500*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			right_arm_controller.send_ARM_ELBOW_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			right_arm_controller.send_stop_motor(RIGHT_ARM_ELBOW_MOTOR);
}
//--------------------------------------------------------------------
void handle_right_arm_forearm_move(char bx, char /*by*/)
{
	// forearm left-right
	if (bx < 32 - ARM_dead_zone_length / 2) // 
		right_arm_controller.send_ARM_FOREARM_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 500*/);
	else
		if (bx > 32 + ARM_dead_zone_length / 2) // 
			right_arm_controller.send_ARM_FOREARM_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - sto
			right_arm_controller.send_stop_motor(RIGHT_ARM_FOREARM_MOTOR);
}
//--------------------------------------------------------------------
void handle_right_arm_wrist_move(char /*bx*/, char by)
{
	// wrist
	if (by < 32 - ARM_dead_zone_length / 2) // 
		right_arm_controller.send_ARM_WRIST_MOTOR_move(ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 500*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			right_arm_controller.send_ARM_WRIST_MOTOR_move(-ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 500*/);
		else// inside dead zone - stop
			right_arm_controller.send_stop_motor(RIGHT_ARM_WRIST_MOTOR);
}
//--------------------------------------------------------------------
void handle_right_arm_gripper_move(char /*bx*/, char /*by*/)
{
	/*
	// gripper
	if (by < 32 - ARM_dead_zone_length / 2) //
		right_arm_controller.send_ARM_WRIST_MOTOR_move(ARM_MOTOR_NUM_STEPS, (32 - by) * STEP_SPEED_ARM, 500);
	else
		if (by > 32 + ARM_dead_zone_length / 2) //
			right_arm_controller.send_ARM_GRIPPER_MOTOR_move(-ARM_MOTOR_NUM_STEPS, (by - 32) * STEP_SPEED_ARM, 500);
		else// inside dead zone - stop
			right_arm_controller.send_stop_motor(LEFT_ARM_GRIPPER_MOTOR);
			*/
}
//--------------------------------------------------------------------


void handle_head_rotate(char bx, char by)
{
	// head right left
	if (bx > 32 + ARM_dead_zone_length / 2) // 
		jenny5_head_controller.send_HEAD_MOTOR_NECK_move(ARM_MOTOR_NUM_STEPS/*, (bx - 32) * STEP_SPEED_ARM, 1*/);
	else
		if (bx < 32 - ARM_dead_zone_length / 2) // 
			jenny5_head_controller.send_HEAD_MOTOR_NECK_move(-ARM_MOTOR_NUM_STEPS/*, (32 - bx) * STEP_SPEED_ARM, 1*/);
		else// inside dead zone - stop
			jenny5_head_controller.send_stop_motor(HEAD_MOTOR_NECK);

	// head up down
	if (by < 32 - ARM_dead_zone_length / 2) // 
		jenny5_head_controller.send_HEAD_MOTOR_FACE_move(ARM_MOTOR_NUM_STEPS/*, (32 - by) * STEP_SPEED_ARM, 1*/);
	else
		if (by > 32 + ARM_dead_zone_length / 2) // 
			jenny5_head_controller.send_HEAD_MOTOR_FACE_move(-ARM_MOTOR_NUM_STEPS/*, (by - 32) * STEP_SPEED_ARM, 1*/);
		else// inside dead zone - stop
			jenny5_head_controller.send_stop_motor(HEAD_MOTOR_FACE);
}
//--------------------------------------------------------------------
void stop_robot(void)
{
	robot_stopped = true;

	if (face_tracking_thread) {
		face_tracking_thread->join();
		delete face_tracking_thread;
		face_tracking_thread = NULL;
	}

	if (follow_person_thread) {
		follow_person_thread->join();
		delete follow_person_thread;
		follow_person_thread = NULL;
	}

	if (platform_controller.is_connected()) {
		platform_controller.stop_motors();
		//platform_controller.disconnect();
	}
	if (leg_controller.is_connected())
		leg_controller.stop_motors();

	char error_string[1000];
	
	if (left_arm_controller.is_connected())
		left_arm_controller.stop_motors(error_string);
	if (right_arm_controller.is_connected())
		right_arm_controller.stop_motors(error_string);
	if (jenny5_head_controller.is_connected())
		jenny5_head_controller.send_stop_motors();

}
//--------------------------------------------------------------------
void disconnect_robot(void)
{
	platform_controller.disconnect();
	leg_controller.disconnect();
	left_arm_controller.disconnect();
	right_arm_controller.disconnect();
	jenny5_head_controller.disconnect();
	LIDAR_controller.disconnect();
}
//--------------------------------------------------------------------
bool connect_and_setup_left_arm(void)
{
	char error_string[1000];
	int error_index;

	if (!left_arm_controller.is_connected()) {
		error_index = left_arm_controller.connect(LEFT_ARM_COM_PORT);
		print_const_message(stdout, left_arm_controller.error_to_string(error_index));
		print_const_message(f_log, left_arm_controller.error_to_string(error_index));
		if (error_index == E_OK) {

			if (!left_arm_controller.setup(error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				return false;
			}
			else {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				return true;
			}
		}
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------
bool connect_and_setup_right_arm(void)
{
	char error_string[1000];
	int error_index;

	if (!right_arm_controller.is_connected()) {
		error_index = right_arm_controller.connect(RIGHT_ARM_COM_PORT);
		print_const_message(stdout, right_arm_controller.error_to_string(error_index));
		print_const_message(f_log, right_arm_controller.error_to_string(error_index));
		if (error_index == E_OK) {

			if (!right_arm_controller.setup(error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				return false;
			}
			else {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				return true;
			}
		}
		else
			return false;
	}
	return true;
}
//--------------------------------------------------------------------
void show_image(char* window_name, cv::Mat &image)
{
	imshow(window_name, image); // display the result
	cv::waitKey(1);
}
//--------------------------------------------------------------------
void current_time_to_string(char *str_result)
{
	strcpy(str_result, "");

	time_t _current_time = time(NULL);

	struct tm * ptm;

	ptm = localtime(&_current_time);

	strftime(str_result, 100, "%Y %m %d %H %M %S", ptm);
}
//--------------------------------------------------------------------
void print_message(FILE *f, char* message)
{
	char log_time[1000];
	current_time_to_string(log_time);
	strcat(log_time, ":");
	fprintf(f, log_time);
	fprintf(f, message);
	fflush(f);
}
//--------------------------------------------------------------------
void print_const_message(FILE* f, const char* message)
{
	char log_time[1000];
	current_time_to_string(log_time);
	strcat(log_time, ":");
	fprintf(f, log_time);
	fprintf(f, message);
	fflush(f);
}
//--------------------------------------------------------------------
void to_log(char* str)
{
	print_message(stdout, str);
}
//-------------------------------------------------------------------
bool stop_function(void)
{
	return robot_stopped;
}
//--------------------------------------------------------------------
int process_command(unsigned char bx, unsigned char by)
{
	char message[1000];
	int error_index;
	char error_string[1000];

//	int command_result;

	char is_command = !(by & (1 << 6)); // 2nd bit is 0
	if (is_command) {

		sprintf(message, "command 1 = %d command 2 = %d\n", bx, by);
		print_message(stdout, message);
		print_message(f_log, message);

		switch (by) {// connect to platform and leg
		case CONNECT_TO_ROBOT:
			// PLATFORM CONNECT
			if (platform_controller.connect(PLATFORM_COM_PORT) != E_OK) {
				print_const_message(stdout, CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR);
				print_const_message(f_log, CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR);
			}
			else {
				sprintf(message, "Connected to platform!\n");
				print_message(stdout, message);
				print_message(f_log, message);
			}
			// LEG CONNECT
			if (leg_controller.connect(LEG_COM_PORT) != E_OK) {
				print_const_message(stdout, CANNOT_CONNECT_TO_JENNY5_LEG_STR);
				print_const_message(f_log, CANNOT_CONNECT_TO_JENNY5_LEG_STR);
			}
			else {
				sprintf(message, "Connected to leg!\n");
				print_message(stdout, message);
				print_message(f_log, message);
			}
			// LEFT ARM CONNECT
			connect_and_setup_left_arm();
			// RIGHT ARM CONNECT
			connect_and_setup_right_arm();

			// HEAD CONNECT
			error_index = jenny5_head_controller.connect(HEAD_COM_PORT);
			print_const_message(stdout, jenny5_head_controller.error_to_string(error_index));
			print_const_message(f_log, jenny5_head_controller.error_to_string(error_index));
			if (error_index == E_OK) {

				if (!jenny5_head_controller.setup(error_string)) {
					print_message(stdout, error_string);
					print_message(f_log, error_string);
				}
			}

			move_mode = PAUSED_STATE;
			print_const_message(stdout, "PAUSED STATE\n");
			print_const_message(f_log, "PAUSED STATE\n");
			face_tracking_thread = NULL;
			follow_person_thread = NULL;
			break;
		case DISCONNECT_FROM_ROBOT:
			stop_robot();
			disconnect_robot();

			disconnected = true;
			move_mode = PAUSED_STATE;

			sprintf(message, "Disconnected from robot!\n");
			print_message(stdout, message);
			print_message(f_log, message);

			break;
		case PAUSE_PLATFORM_COMMAND:
			// pause platform motors
			stop_robot();

			move_mode = PAUSED_STATE;
			print_const_message(stdout, "PAUSED STATE\n");
			print_const_message(f_log, "PAUSED STATE\n");

			break;
		case POWER_DISABLE_COMMAND:
			stop_robot();
			jenny5_head_controller.send_disable_motors();
			left_arm_controller.send_disable_motors();
			right_arm_controller.send_disable_motors();

			move_mode = PAUSED_STATE;
			print_const_message(stdout, "PAUSED STATE & DISABLE POWER\n");
			print_const_message(f_log, "PAUSED STATE & DISABLE POWER\n");
			break;
		case LEG_MOVE_COMMAND:
			// leg up
			if (!leg_controller.is_connected()) { // not connected; try to connect now
				if (leg_controller.connect(LEG_COM_PORT) != E_OK) {
					print_const_message(stdout, CANNOT_CONNECT_TO_JENNY5_LEG_STR);
					print_const_message(f_log, CANNOT_CONNECT_TO_JENNY5_LEG_STR);
				}
				else {
					sprintf(message, "Connected to leg!\n");
					print_message(stdout, message);
					print_message(f_log, message);
				}
			}
			stop_robot();
			move_mode = LEG_MOVE_STATE;
			print_const_message(stdout, "LEG MOVE STATE\n");
			print_const_message(f_log, "LEG MOVE STATE\n");

			break;
		case ROTATE_COMMAND:
			// rotate
			platform_controller.disconnect();
			if (!platform_controller.is_connected()) { // not connected ; try to connect now
				if (platform_controller.connect(PLATFORM_COM_PORT) != E_OK) {
					print_const_message(stdout, CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR);
					print_const_message(f_log, CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR);
				}
				else {
					sprintf(message, "Connected to platform!\n");
					print_message(stdout, message);
					print_message(f_log, message);
				}
			}

			stop_robot();
			move_mode = PLATFORM_ROTATE_STATE;
			print_const_message(stdout, "ROTATE PLATFORM STATE\n");
			print_const_message(f_log, "ROTATE PLATFORM STATE\n");

			break;
		case NAVIGATE_COMMAND:
			// navigate
			platform_controller.disconnect();
			if (!platform_controller.is_connected()) { // not connected ; try to connect now
				if (platform_controller.connect(PLATFORM_COM_PORT) != E_OK) {
					print_const_message(stdout, CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR);
					print_const_message(f_log, CANNOT_CONNECT_TO_JENNY5_PLATFORM_STR);
				}
				else {
					sprintf(message, "Connected to platform!\n");
					print_message(stdout, message);
					print_message(f_log, message);
				}
			}
			move_mode = PLATFORM_NAVIGATE_STATE;
			print_const_message(stdout, "PLATFORM_NAVIGATE STATE\n");
			print_const_message(f_log, "PLATFORM_NAVIGATE STATE\n");

			break;

			// move first LEFT arm motor
		case LEFT_ARM_BODY_LEFT_RIGHT_COMMAND:
			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_BODY_LEFT_RIGHT_MOVE_STATE;
			print_const_message(stdout, "LEFT_ARM_BODY_LEFT_RIGHT_MOVE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_BODY_LEFT_RIGHT_MOVE_STATE\n");

			break;
		case LEFT_ARM_UP_DOWN_COMMAND:
			// move second arm motor
			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_UP_DOWN_MOVE_STATE;
			print_const_message(stdout, "LEFT_ARM_UP_DOWN_MOVE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_UP_DOWN_MOVE_STATE\n");

			break;
		case LEFT_ARM_ROTATE_COMMAND:
			// move rotate motor
			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_ROTATE_STATE;
			print_const_message(stdout, "LEFT_ARM_ROTATE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_ROTATE_STATE\n");

			break;

		case LEFT_ARM_ELBOW_MOVE_COMMAND:
			// move elbow motor
			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_ELBOW_MOVE_STATE;
			print_const_message(stdout, "LEFT_ARM_ELBOW_MOVE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_ELBOW_MOVE_STATE\n");

			break;
		case LEFT_ARM_FOREARM_MOVE_COMMAND:
			// move forearm motor
			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_FOREARM_MOVE_STATE;
			print_const_message(stdout, "LEFT_ARM_FOREARM_MOVE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_FOREARM_MOVE_STATE\n");

			break;
		case LEFT_ARM_WRIST_MOVE_COMMAND:
			// move wrist motor
			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_WRIST_MOVE_STATE;
			print_const_message(stdout, "LEFT_ARM_WRIST_MOVE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_WRIST_MOVE_STATE\n");

			break;
		case LEFT_ARM_GRIPPER_MOVE_COMMAND:
			// move gripper motor

			connect_and_setup_left_arm();

			stop_robot();
			move_mode = LEFT_ARM_GRIPPER_MOVE_STATE;
			print_const_message(stdout, "LEFT_ARM_GRIPPER_MOVE_STATE\n");
			print_const_message(f_log, "LEFT_ARM_GRIPPER_MOVE_STATE\n");

			break;
		case LEFT_ARM_READ_SENSORS_COMMAND:
			connect_and_setup_left_arm();
			left_arm_controller.read_all_sensors();
		//	stop_robot();
			//move_mode = LEFT_ARM_GRIPPER_MOVE_STATE;
			//print_message(stdout, "LEFT_ARM_GRIPPER_MOVE_STATE\n");
			//print_message(f_log, "LEFT_ARM_GRIPPER_MOVE_STATE\n");
			break;



			// move first RIGHT arm motor
		case RIGHT_ARM_BODY_LEFT_RIGHT_COMMAND:
			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_BODY_LEFT_RIGHT_MOVE_STATE;
			print_const_message(stdout, "RIGHT_ARM_BODY_LEFT_RIGHT_MOVE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_BODY_LEFT_RIGHT_MOVE_STATE\n");

			break;
		case RIGHT_ARM_UP_DOWN_COMMAND:
			// move second arm motor
			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_UP_DOWN_MOVE_STATE;
			print_const_message(stdout, "RIGHT_ARM_UP_DOWN_MOVE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_UP_DOWN_MOVE_STATE\n");

			break;
		case RIGHT_ARM_ROTATE_COMMAND:
			// move rotate motor
			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_ROTATE_STATE;
			print_const_message(stdout, "RIGHT_ARM_ROTATE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_ROTATE_STATE\n");

			break;

		case RIGHT_ARM_ELBOW_MOVE_COMMAND:
			// move elbow motor
			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_ELBOW_MOVE_STATE;
			print_const_message(stdout, "RIGHT_ARM_ELBOW_MOVE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_ELBOW_MOVE_STATE\n");

			break;
		case RIGHT_ARM_FOREARM_MOVE_COMMAND:
			// move forearm motor
			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_FOREARM_MOVE_STATE;
			print_const_message(stdout, "RIGHT_ARM_FOREARM_MOVE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_FOREARM_MOVE_STATE\n");

			break;
		case RIGHT_ARM_WRIST_MOVE_COMMAND:
			// move wrist motor
			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_WRIST_MOVE_STATE;
			print_const_message(stdout, "RIGHT_ARM_WRIST_MOVE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_WRIST_MOVE_STATE\n");

			break;
		case RIGHT_ARM_GRIPPER_MOVE_COMMAND:
			// move gripper motor

			connect_and_setup_right_arm();

			stop_robot();
			move_mode = RIGHT_ARM_GRIPPER_MOVE_STATE;
			print_const_message(stdout, "RIGHT_ARM_GRIPPER_MOVE_STATE\n");
			print_const_message(f_log, "RIGHT_ARM_GRIPPER_MOVE_STATE\n");

			break;
		case RIGHT_ARM_READ_SENSORS_COMMAND:
			connect_and_setup_right_arm();
			right_arm_controller.read_all_sensors();
			//	stop_robot();
				//move_mode = RIGHT_ARM_GRIPPER_MOVE_STATE;
				//print_message(stdout, "RIGHT_ARM_GRIPPER_MOVE_STATE\n");
				//print_message(f_log, "RIGHT_ARM_GRIPPER_MOVE_STATE\n");
			break;



		case HEAD_ROTATE_COMMAND:
			// head rotate

			if (!jenny5_head_controller.is_connected()) {
				// not connected; try to connect now
				error_index = jenny5_head_controller.connect(HEAD_COM_PORT);
				print_const_message(stdout, jenny5_head_controller.error_to_string(error_index));
				print_const_message(f_log, jenny5_head_controller.error_to_string(error_index));
				if (error_index == E_OK) {
					if (!jenny5_head_controller.setup(error_string)) {
						print_message(stdout, error_string);
						print_message(f_log, error_string);
					}
				}
			}

			stop_robot();
			move_mode = HEAD_ROTATE_STATE;
			print_const_message(stdout, "HEAD_ROTATE_STATE\n");
			print_const_message(f_log, "HEAD_ROTATE_STATE\n");

			break;
		case FACE_TRACKING_COMMAND:

			// face tracking command
			stop_robot();

			move_mode = FACE_TRACKING_STATE;

//			char error_string[1000];

			if (!jenny5_head_controller.cam.open(HEAD_CAMERA_INDEX)) {	// link it to the device [0 = default cam] (USBcam is default 'cause I disabled the onbord one IRRELEVANT!)
				print_const_message(stdout, "Couldn't open head's video camera!\n");
				print_const_message(f_log, "Couldn't open head's video camera!\n");
				move_mode = PAUSED_STATE;
			}
			else {
				print_const_message(stdout, "Head camera connection succceded.\n");
				print_const_message(f_log, "Head camera connection succceded.\n");
			}
			// initialization

			if (!init_face_classifier(face_classifier, error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				move_mode = PAUSED_STATE;
			}
			else {
				print_const_message(stdout, "Face classifier initialization succceded.\n");
				print_const_message(f_log, "Face classifier initialization succceded.\n");
			}
			//  home
			if (!jenny5_head_controller.home_all_motors(error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				move_mode = PAUSED_STATE;
			}
			else {
				print_const_message(stdout, "Head home succceded.\n");
				print_const_message(f_log, "Head home succceded.\n");
			}
			// run the face tracking
			if (move_mode == FACE_TRACKING_STATE) {
				robot_stopped = false;
				//cv::namedWindow("Head camera", cv::WINDOW_AUTOSIZE); // window to display the results
				print_const_message(stdout, "FACE_TRACKING_STATE\n");
				print_const_message(f_log, "FACE_TRACKING_STATE\n");

//				face_tracking_thread = new std::thread(head_face_follow, &jenny5_head_controller, &face_classifier, to_log, stop_function, (char*)NULL);
			}

			break;

		case FOLLOW_PERSON_COMMAND:
			// follow person command
			stop_robot();
			move_mode = FOLLOW_PERSON_STATE;

			if (!jenny5_head_controller.cam.open(HEAD_CAMERA_INDEX)) {	// link it to the device [0 = default cam] (USBcam is default 'cause I disabled the onbord one IRRELEVANT!)
				print_const_message(stdout, "Couldn't open head's video camera!\n");
				print_const_message(f_log, "Couldn't open head's video camera!\n");
				move_mode = PAUSED_STATE;
			}
			else {
				print_const_message(stdout, "Head camera connection succceded.\n");
				print_const_message(f_log, "Head camera connection succceded.\n");
			}
			// initialization

			if (!init_face_classifier(face_classifier, error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				move_mode = PAUSED_STATE;
			}
			else {
				print_const_message(stdout, "Face classifier initialization succceded.\n");
				print_const_message(f_log, "Face classifier initialization succceded.\n");
			}
			//  home
			if (!jenny5_head_controller.home_all_motors(error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
				move_mode = PAUSED_STATE;
			}
			else {
				print_const_message(stdout, "Head home succceded.\n");
				print_const_message(f_log, "Head home succceded.\n");
			}

			if (!LIDAR_controller.is_connected()) {
				error_index = LIDAR_controller.connect(LIDAR_COM_PORT);
				print_const_message(stdout, LIDAR_controller.error_to_string(error_index));
				print_const_message(f_log, LIDAR_controller.error_to_string(error_index));
				if (error_index != E_OK) {
					move_mode = PAUSED_STATE;
				}

				// setup
				if (!LIDAR_controller.setup(error_string)) {
					print_message(stdout, error_string);
					print_message(f_log, error_string);
					move_mode = PAUSED_STATE;
				}
				else {
					print_const_message(stdout, "Setup LIDAR OK.\n");
					print_const_message(f_log, "Setup LIDAR OK.\n");
				}
			}
			// run the follow person algorithm
			if (move_mode == FOLLOW_PERSON_STATE) {
				robot_stopped = false;
				print_const_message(stdout, "FOLLOW_PERSON_STATE\n");
				print_const_message(f_log, "FOLLOW_PERSON_STATE\n");

//				follow_person_thread = new std::thread(follow_person, &jenny5_head_controller, &LIDAR_controller, &platform_controller, &face_classifier, to_log, stop_function, (char*)NULL, (char*)NULL);
			}

			break;

		case CAPTURE_HEAD_CAMERA:
			return jenny5_head_controller.capture_camera_save_to_disk_and_release("c:/jenny5/jenny5-html5/client/www/head.jpg");
				
			break;

		case CAPTURE_LEFT_ARM_CAMERA:
			//if (!capture_camera(LEFT_ARM_CAMERA_INDEX))
				//return 0;
			break;

		case CAPTURE_RIGHT_ARM_CAMERA:
			//if (!capture_camera(RIGHT_ARM_CAMERA_INDEX))
				//return 0;
			break;

		case LEFT_ARM_WAVE_COMMAND:
			// move gripper motor

			stop_robot();
			connect_and_setup_left_arm();

			move_mode = WAVE_LEFT_ARM_STATE;
			print_const_message(stdout, "WAVING LEFT ARM STATE\n");
			print_const_message(f_log, "WAVING LEFT ARM STATE\n");

			if (!left_arm_controller.wave_hand(error_string)){
				print_message(stdout, error_string);
				print_message(f_log, error_string);
			}

			print_const_message(stdout, "WAVING LEFT ARM DONE\n");
			print_const_message(f_log, "WAVING LEFT ARM DONE\n");

			break;

		case RIGHT_ARM_WAVE_COMMAND:
			// move gripper motor

			stop_robot();
			connect_and_setup_right_arm();

			move_mode = WAVE_RIGHT_ARM_STATE;
			print_const_message(stdout, "WAVING RIGHT ARM STATE\n");
			print_const_message(f_log, "WAVING RIGHT ARM STATE\n");

			if (!right_arm_controller.wave_hand(error_string)) {
				print_message(stdout, error_string);
				print_message(f_log, error_string);
			}

			print_const_message(stdout, "WAVING RIGHT ARM DONE\n");
			print_const_message(f_log, "WAVING RIGHT ARM DONE\n");

			break;
		}// end switch commands
	}
	else { // movement data
		char sensor_bx = bx & 0x3F;// last 6 bits of the data
		char sensor_by = by & 0x3F;// last 6 bits of the data

		sprintf(message, "data 1 = %d data 2 = %d; bx = %d by = %d\n", sensor_bx, sensor_by, bx, by);
		print_message(stdout, message);
		print_message(f_log, message);

		switch (move_mode) {
		case PLATFORM_NAVIGATE_STATE:
			if (platform_controller.is_connected()) {
				int result = handle_navigate(sensor_bx, sensor_by);

				if (result) {
					sprintf(message, "Motor command error = %d\n", result);
					print_message(stdout, message);
					print_message(f_log, message);
				}
				
				double motor1_amps, motor2_amps;
				platform_controller.roboclaw_controller.get_motors_current_consumption(motor1_amps, motor2_amps);
				sprintf(message, "platform motor1 amps = %lf; motor2 amps = %lf\n", motor1_amps, motor2_amps);
				print_message(stdout, message);
				print_message(f_log, message);

				double board_temperature;
				board_temperature = platform_controller.roboclaw_controller.get_board_temperature();
				sprintf(message, "platform board temperature = %lf\n", board_temperature);
				print_message(stdout, message);
				print_message(f_log, message);
				
			}

			break;
		case PLATFORM_ROTATE_STATE:
			if (platform_controller.is_connected()) {
				int result = handle_platform_rotate(sensor_bx, sensor_by);

				if (result) {
					sprintf(message, "Motor command error = %d\n", result);
					print_message(stdout, message);
					print_message(f_log, message);
				}
				
				double motor1_amps, motor2_amps;
				platform_controller.roboclaw_controller.get_motors_current_consumption(motor1_amps, motor2_amps);
				sprintf(message, "platform motor1 amps = %lf; motor2 amps = %lf\n", motor1_amps, motor2_amps);
				print_message(stdout, message);
				print_message(f_log, message);

				double board_temperature;
				board_temperature = platform_controller.roboclaw_controller.get_board_temperature();
				sprintf(message, "platform board temperature = %lf\n", board_temperature);
				print_message(stdout, message);
				print_message(f_log, message);
				
			}
			break;

		case LEG_MOVE_STATE:
			if (leg_controller.is_connected()) {

				handle_leg_move(sensor_bx, sensor_by);

				double motor1_amps, motor2_amps;
				leg_controller.roboclaw_controller.get_motors_current_consumption(motor1_amps, motor2_amps);
				sprintf(message, "leg motor1 amps = %lf; motor2 amps = %lf\n", motor1_amps, motor2_amps);
				print_message(stdout, message);
				print_message(f_log, message);

				double board_temperature;
				board_temperature = leg_controller.roboclaw_controller.get_board_temperature();
				sprintf(message, "leg board temperature = %lf\n", board_temperature);
				print_message(stdout, message);
				print_message(f_log, message);
			}

			break;

		case LEFT_ARM_BODY_LEFT_RIGHT_MOVE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_body_left_right_move(sensor_bx, sensor_by);
			}

			break;

		case LEFT_ARM_UP_DOWN_MOVE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_up_down_move(sensor_bx, sensor_by);
			}
			break;

		case LEFT_ARM_ROTATE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_rotate_move(sensor_bx, sensor_by);
			}
			break;
		case LEFT_ARM_ELBOW_MOVE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_elbow_move(sensor_bx, sensor_by);
			}
			break;
		case LEFT_ARM_FOREARM_MOVE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_forearm_move(sensor_bx, sensor_by);
			}
			break;

		case LEFT_ARM_WRIST_MOVE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_wrist_move(sensor_bx, sensor_by);
			}
			break;

		case LEFT_ARM_GRIPPER_MOVE_STATE:
			if (left_arm_controller.is_connected()) {
				handle_left_arm_gripper_move(sensor_bx, sensor_by);
			}
			break;


		case RIGHT_ARM_BODY_LEFT_RIGHT_MOVE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_body_left_right_move(sensor_bx, sensor_by);
			}

			break;

		case RIGHT_ARM_UP_DOWN_MOVE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_up_down_move(sensor_bx, sensor_by);
			}
			break;

		case RIGHT_ARM_ROTATE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_rotate_move(sensor_bx, sensor_by);
			}
			break;
		case RIGHT_ARM_ELBOW_MOVE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_elbow_move(sensor_bx, sensor_by);
			}
			break;
		case RIGHT_ARM_FOREARM_MOVE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_forearm_move(sensor_bx, sensor_by);
			}
			break;

		case RIGHT_ARM_WRIST_MOVE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_wrist_move(sensor_bx, sensor_by);
			}
			break;

		case RIGHT_ARM_GRIPPER_MOVE_STATE:
			if (right_arm_controller.is_connected()) {
				handle_right_arm_gripper_move(sensor_bx, sensor_by);
			}
			break;

		case HEAD_ROTATE_STATE:
			if (jenny5_head_controller.is_connected()) {
				handle_head_rotate(sensor_bx, sensor_by);
			}
			break;
		}
	}
	return E_OK;
}