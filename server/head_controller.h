// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#ifndef head_controller_H
#define head_controller_H

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "jenny5_arduino_controller.h"

#define HEAD_MOTOR_NECK 0
#define HEAD_MOTOR_FACE 1

#define HEAD_POTENTIOMETER_NECK_INDEX 0
#define HEAD_POTENTIOMETER_FACE_INDEX 1
#define HEAD_ULTRASONIC_FACE_INDEX 0

#define HEAD_CAMERA_INDEX 0


// head
#define _head_horizontal_motor_potentiometer_min 160
#define _head_horizontal_motor_potentiometer_max 730
#define _head_horizontal_motor_potentiometer_home 460

#define _head_vertical_motor_potentiometer_min 332
#define _head_vertical_motor_potentiometer_max 832
#define _head_vertical_motor_potentiometer_home 515

#define HEAD_MOTOR_GEAR_REDUCTION 27

#define HEAD_RADIUS_TO_REVERT 70
#define HEAD_RADIUS_TOLERANCE 20

#define CAM_PIXELS_TOLERANCE 100


#define CANNOT_CONNECT_TO_JENNY5_HEAD_STR "CANNOT_CONNECT_TO_JENNY5_HEAD\n" 
#define CANNOT_CONNECT_TO_JENNY5_HEAD_ERROR 1

#define Head_does_not_respond_STR "Head does not respond! Game over!\n"
#define Head_does_not_respond_ERROR 2

#define CANNOT_CONNECT_TO_HEAD_CAMERA_STR "CANNOT_CONNECT_TO_HEAD_CAMERA\n"
#define CANNOT_CONNECT_TO_HEAD_CAMERA_ERROR 3

#define CANNOT_WRITE_HEAD_CAMERA_IMAGE_TO_DISK_STR "CANNOT_WRITE_HEAD_CAMERA_IMAGE_TO_DISK\n"
#define CANNOT_WRITE_HEAD_CAMERA_IMAGE_TO_DISK_ERROR 4

#define CANNOT_CAPTURE_IMAGE_FROM_HEAD_CAMERA 5

#define NOT_CONNECTED_TO_HE

#define Connected_to_head_STR "Connected to head\n"
//-------------------------------------------------------------
class t_head_controller {
public:
	cv::VideoCapture cam;
	t_jenny5_arduino_controller head_arduino_controller;
	t_head_controller();

	int connect(int HEAD_COM_PORT);
	bool is_connected(void);
	void disconnect(void);
	bool setup(char* error_string);

	void send_get_arduino_firmware_version(void);

	void send_get_sensors_value(void);
	void send_home_all(void);
	void send_neck_home(void);
	void send_face_home(void);

	void send_neck_to_sensor_position(int head_neck_new_position);
	void send_face_to_sensor_position(int head_face_new_position);

	void send_HEAD_MOTOR_NECK_move(int num_steps/*, int speed, int accelleration*/);
	void send_HEAD_MOTOR_FACE_move(int num_steps/*, int speed, int accelleration*/);

	void send_stop_motor(int motor_index);
	void send_stop_motors(void);

	// send home and blocks until returns
	bool home_all_motors(char* error_string);
	void send_disable_motors(void);

	char *error_to_string(int error);

	bool open_camera(void);
	void release_camera(void);
	int capture_camera_and_save_to_disk(char *file_name);

	int capture_camera_save_to_disk_and_release(char *file_name);
};

extern t_head_controller jenny5_head_controller;

#endif