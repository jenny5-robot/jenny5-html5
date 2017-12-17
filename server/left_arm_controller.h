// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------
#ifndef left_arm_controller_H
#define left_arm_controller_H

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "jenny5_arduino_controller.h"

#define LEFT_ARM_BODY_MOTOR 0
#define LEFT_ARM_SHOULDER_UP_DOWN_MOTOR 1
#define LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 2
#define LEFT_ARM_ELBOW_MOTOR 3
#define LEFT_ARM_FOREARM_MOTOR 4
#define LEFT_ARM_GRIPPER_MOTOR 5

#define LEFT_ARM_BODY_POTENTIOMETER_INDEX 0
#define LEFT_ARM_SHOULDER_UP_DOWN_POTENTIOMETER_INDEX 1
#define LEFT_ARM_SHOULDER_LEFT_RIGHT_POTENTIOMETER_INDEX 2
#define LEFT_ARM_ELBOW_POTENTIOMETER_INDEX 3
#define LEFT_ARM_FOREARM_POTENTIOMETER_INDEX 4
#define LEFT_ARM_GRIPPER_BUTTON_INDEX 0
#define LEFT_ARM_GRIPPER_INFRARED_INDEX 0

#define LEFT_ARM_GRIPPER_BUTTON_PIN 15

// left arm
#define _potentiometer_min_LEFT_ARM_BODY_MOTOR 380
#define _potentiometer_max_LEFT_ARM_BODY_MOTOR 800
#define _potentiometer_home_LEFT_ARM_BODY_MOTOR 640

#define _potentiometer_min_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR 270// up
#define _potentiometer_max_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR 850// down
#define _potentiometer_home_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR 830// 

#define _potentiometer_min_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 250
#define _potentiometer_max_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 730
#define _potentiometer_home_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR 260

#define _potentiometer_min_LEFT_ARM_ELBOW_MOTOR 480
#define _potentiometer_max_LEFT_ARM_ELBOW_MOTOR 940
#define _potentiometer_home_LEFT_ARM_ELBOW_MOTOR 630

#define _potentiometer_min_LEFT_ARM_FOREARM_MOTOR 230
#define _potentiometer_max_LEFT_ARM_FOREARM_MOTOR 720
#define _potentiometer_home_LEFT_ARM_FOREARM_MOTOR 440

#define LEFT_ARM_CAMERA_INDEX 1


#define CANNOT_CONNECT_TO_JENNY5_LEFT_ARM_STR "CANNOT CONNECT TO JENNY5 LEFT ARM\n" 
#define CANNOT_CONNECT_TO_JENNY5_LEFT_ARM_ERROR 1

#define LEFT_ARM_does_not_respond_STR "LEFT_ARM does not respond! Game over!\n"
#define LEFT_ARM_does_not_respond_ERROR 2

#define Connected_to_LEFT_ARM_STR "Connected to LEFT_ARM\n"

#define ARM_DEFAULT_MOTOR_SPEED 400
#define ARM_DEFAULT_MOTOR_ACCELERATION 200

//-------------------------------------------------------------------------------
class t_left_arm_controller {
public:
	cv::VideoCapture left_arm_cam;
	t_jenny5_arduino_controller arduino_controller;
	cv::CascadeClassifier face_classifier;
	t_left_arm_controller(void);

	int connect(int LEF_ARM_COM_PORT);
	bool is_connected(void);
	void disconnect(void);
	bool setup(char* error_string);

	void send_get_arduino_firmware_version(void);

	void send_get_sensors_value(void);

	void send_LEFT_ARM_BODY_MOTOR_home(void);
	void send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_home(void);
	void send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_home(void);
	void send_LEFT_ARM_ELBOW_MOTOR_home(void);
	void send_LEFT_ARM_FOREARM_MOTOR_home(void);
	void send_LEFT_ARM_GRIPPER_MOTOR_home(void);

	void send_LEFT_ARM_BODY_MOTOR_to_sensor_position(int new_position);
	void send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_to_sensor_position(int new_position);
	void send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_to_sensor_position(int new_position);
	void send_LEFT_ARM_ELBOW_MOTOR_to_sensor_position(int new_position);
	void send_LEFT_ARM_FOREARM_MOTOR_to_sensor_position(int new_position);
	void send_LEFT_ARM_GRIPPER_MOTOR_start_open(void);
	void send_LEFT_ARM_GRIPPER_MOTOR_stop_open(void);

	void send_LEFT_ARM_BODY_MOTOR_move(int num_steps, int speed, int accelleration);
	void send_LEFT_ARM_SHOULDER_UP_DOWN_MOTOR_move(int num_steps, int speed, int accelleration);
	void send_LEFT_ARM_SHOULDER_LEFT_RIGHT_MOTOR_move(int num_steps, int speed, int accelleration);
	void send_LEFT_ARM_ELBOW_MOTOR_move(int num_steps, int speed, int accelleration);
	void send_LEFT_ARM_FOREARM_MOTOR_move(int num_steps, int speed, int accelleration);
	void send_LEFT_ARM_GRIPPER_MOTOR_move(int num_steps, int speed, int accelleration);

	void send_stop_motor(int motor_index);

	bool home_all_motors(char* error_string);
	void send_disable_motors(void);

	void send_stop_motors(void);

	bool wave_hand(char *error_string);

	char *error_to_string(int error);
};
//-------------------------------------------------------------------------------
extern t_left_arm_controller left_arm_controller;

#endif