// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------
#ifndef right_arm_controller_H
#define right_arm_controller_H

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "scufy_lib.h"

#define RIGHT_ARM_BODY_MOTOR 0
#define RIGHT_ARM_SHOULDER_UP_DOWN_MOTOR 1
#define RIGHT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 2
#define RIGHT_ARM_ELBOW_MOTOR 3
#define RIGHT_ARM_FOREARM_MOTOR 4
#define RIGHT_ARM_WRIST_MOTOR 5

#define RIGHT_ARM_BODY_AS5147_INDEX 0
#define RIGHT_ARM_SHOULDER_UP_DOWN_AS5147_INDEX 1
#define RIGHT_ARM_SHOULDER_LEFT_RIGHT_AS5147_INDEX 2
#define RIGHT_ARM_ELBOW_AS5147_INDEX 3
#define RIGHT_ARM_FOREARM_AS5147_INDEX 4
#define RIGHT_ARM_WRIST_AS5147_INDEX 5

#define RIGHT_ARM_GRIPPER_BUTTON_INDEX 0
#define RIGHT_ARM_GRIPPER_INFRARED_INDEX 0

#define RIGHT_ARM_GRIPPER_BUTTON_PIN 15

// RIGHT arm
#define _AS5147_min_RIGHT_ARM_BODY_MOTOR 200
#define _AS5147_max_RIGHT_ARM_BODY_MOTOR 300
#define _AS5147_home_RIGHT_ARM_BODY_MOTOR 250

#define _AS5147_min_RIGHT_ARM_SHOULDER_UP_DOWN_MOTOR 40// up
#define _AS5147_max_RIGHT_ARM_SHOULDER_UP_DOWN_MOTOR 220// down
#define _AS5147_home_RIGHT_ARM_SHOULDER_UP_DOWN_MOTOR 210// 

#define _AS5147_min_RIGHT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 230
#define _AS5147_max_RIGHT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 100
#define _AS5147_home_RIGHT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 300

#define _AS5147_min_RIGHT_ARM_ELBOW_MOTOR 100
#define _AS5147_max_RIGHT_ARM_ELBOW_MOTOR 200
#define _AS5147_home_RIGHT_ARM_ELBOW_MOTOR 150

#define _AS5147_min_RIGHT_ARM_FOREARM_MOTOR 330
#define _AS5147_max_RIGHT_ARM_FOREARM_MOTOR 150
#define _AS5147_home_RIGHT_ARM_FOREARM_MOTOR 100

#define _AS5147_min_RIGHT_ARM_WRIST_MOTOR 300
#define _AS5147_max_RIGHT_ARM_WRIST_MOTOR 100
#define _AS5147_home_RIGHT_ARM_WRIST_MOTOR 50

#define RIGHT_ARM_CAMERA_INDEX 1


#define CANNOT_CONNECT_TO_JENNY5_RIGHT_ARM_STR "CANNOT CONNECT TO JENNY5 RIGHT ARM\n" 
#define CANNOT_CONNECT_TO_JENNY5_RIGHT_ARM_ERROR 1

#define RIGHT_ARM_does_not_respond_STR "RIGHT ARM does not respond! Game over!\n"
#define RIGHT_ARM_does_not_respond_ERROR 2

#define Connected_to_RIGHT_ARM_STR "Connected to RIGHT ARM\n"

#define ARM_DEFAULT_MOTOR_SPEED 1250
#define ARM_DEFAULT_MOTOR_ACCELERATION 1250

#define ARM_DEFAULT_MOTOR_SPEED_NEMA_16 1100

//-------------------------------------------------------------------------------
class t_right_arm_controller {

private:
public:
	cv::VideoCapture right_arm_cam;
	t_scufy_lib arduino_controller;
	cv::CascadeClassifier face_classifier;
	t_right_arm_controller(void);

	int connect(const char* port);
	bool is_connected(void);
	void disconnect(void);
	bool setup(char* error_string);
	bool create_stepper_motors_controller(char* error_string);
	bool create_as5147s_controller(char* error_string);
	bool set_motor_speed_acceleration(int motor_index, int speed, int acceleration, char* error_string);
	bool attach_sensors_to_stepper_motor(int motor_index, int *AS5147_index, int * AS5147_min, int *AS5147_max, int *AS5147_home, int *AS5147_dir, char* error_string);
	bool stop_motor(int motor_index, char* error_string);
	bool stop_motors(char *);

	void send_get_arduino_firmware_version(void);

	void send_get_sensors_value(void);

	void send_ARM_BODY_MOTOR_home(void);
	void send_ARM_SHOULDER_UP_DOWN_MOTOR_home(void);
	void send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_home(void);
	void send_ARM_ELBOW_MOTOR_home(void);
	void send_ARM_FOREARM_MOTOR_home(void);
	void send_ARM_WRIST_MOTOR_home(void);

	void send_ARM_BODY_MOTOR_to_sensor_position(int new_position);
	void send_ARM_SHOULDER_UP_DOWN_MOTOR_to_sensor_position(int new_position);
	void send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(int new_position);
	void send_ARM_ELBOW_MOTOR_to_sensor_position(int new_position);
	void send_ARM_FOREARM_MOTOR_to_sensor_position(int new_position);
	void send_ARM_WRIST_MOTOR_to_sensor_position(int new_position);
	void send_ARM_GRIPPER_MOTOR_start_open(void);
	void send_ARM_GRIPPER_MOTOR_stop_open(void);

	void send_ARM_BODY_MOTOR_move(int num_steps/*, int speed, int accelleration*/);
	void send_ARM_SHOULDER_UP_DOWN_MOTOR_move(int num_steps/*, int speed, int accelleration*/);
	void send_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(int num_steps/*, int speed, int accelleration*/);
	void send_ARM_ELBOW_MOTOR_move(int num_steps/*, int speed, int accelleration*/);
	void send_ARM_FOREARM_MOTOR_move(int num_steps/*, int speed, int accelleration*/);
	void send_ARM_WRIST_MOTOR_move(int num_steps/*, int speed, int accelleration*/);

	void send_stop_motor(int motor_index);

	bool home_all_motors(char* error_string);
	void send_disable_motors(void);

	bool wave_hand(char *error_string);
	bool read_all_sensors(/*char *error_string*/void);

	const char *error_to_string(int error);
};
//-------------------------------------------------------------------------------
extern t_right_arm_controller right_arm_controller;

#endif
