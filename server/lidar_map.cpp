// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "setup_functions.h"
#include "utils.h"
#include "jenny5_arduino_controller.h"
#include "jenny5_defs.h"
#include "lidar_map.h"

using namespace cv;

//----------------------------------------------------------------
void create_and_init_lidar_image(Mat &lidar_image, int image_width, double lidar_map_scale_factor)
{
	//lidar_image.create(image_width, image_width, CV_8UC3);
	//lidar_image.zeros(image_width, image_width, CV_8UC3);
	Point center(image_width / 2, image_width / 2);
	//LIDAR
	circle(lidar_image, center, 10, Scalar(0, 255, 0), 1, 8);
	// robot
	rectangle(lidar_image, Point(center.x - 175 * lidar_map_scale_factor, center.y), Point(center.x + lidar_map_scale_factor * 175, center.y + 600 * lidar_map_scale_factor), Scalar(0, 0, 255));

	char text[100];
	sprintf(text, "scale = %.2lf", lidar_map_scale_factor);
	int font_face = FONT_HERSHEY_SCRIPT_SIMPLEX;
	cv::Point text_position(10, 25);
	cv::putText(lidar_image, text, text_position, font_face, 1, Scalar::all(255), 1, 8);
}
//----------------------------------------------------------------
int lidar_map(t_lidar_controller &LIDAR_controller, int lidar_com_port, f_log_callback to_log)
{
	// setup
	char error_string[1000];
	int error_index = LIDAR_controller.connect(lidar_com_port);
	if (error_index != E_OK) {
		to_log(LIDAR_controller.error_to_string(error_index));
		return -1;
	}
	else
		to_log("Connection to LIDAR OK.\n");

	// setup
	if (!LIDAR_controller.setup(error_string)) {
		to_log(error_string);
		return -1;
	}
	else
		to_log("Setup LIDAR OK.\n");

	LIDAR_controller.arduino_controller.send_set_LIDAR_motor_speed_and_acceleration(60, 100);
	LIDAR_controller.arduino_controller.send_LIDAR_go();

	int image_width = 600;
	Point center(image_width / 2, image_width / 2);
//	Mat lidar_image;
	Mat lidar_image = Mat::zeros(image_width, image_width, CV_8UC3);

	namedWindow("LIDAR map", WINDOW_AUTOSIZE);

	t_lidar_user_data user_data;
	user_data.lidar_image = &lidar_image;
	user_data.image_width = image_width;
	for (int i = 0; i < LIDAR_NUM_STEPS; i++)
		LIDAR_controller.lidar_distances[i] = 0;

	user_data.lidar_map_scale_factor = 0.1;
	user_data.LIDAR_controller = &LIDAR_controller;

	create_and_init_lidar_image(lidar_image, image_width, user_data.lidar_map_scale_factor);
	
	setMouseCallback("LIDAR map", on_lidar_mouse_event, &user_data);

	to_log("Now running the main loop. Press Escape when want to exit!\n");
	bool active = true;

	while (active) {        // starting infinit loop

		if (!LIDAR_controller.arduino_controller. update_commands_from_serial())
			Sleep(5); // no new data from serial ... we make a little pause so that we don't kill the processor

		bool at_least_one_new_LIDAR_distance = update_lidar_image(LIDAR_controller, lidar_image, image_width, user_data.lidar_map_scale_factor, to_log);
		imshow("LIDAR map", lidar_image);

		if (waitKey(1) == VK_ESCAPE)  // break the loop
			active = false;
	}

	LIDAR_controller.arduino_controller.send_LIDAR_stop();

	LIDAR_controller.arduino_controller.close_connection();

	destroyWindow("LIDAR map");

	return 0;
}
//----------------------------------------------------------------
void on_lidar_mouse_event(int event, int x, int y, int flags, void *userdata)
{
	if (event == cv::EVENT_MOUSEWHEEL) {
		int delta = cv::getMouseWheelDelta(flags);

		t_lidar_user_data* user_data = (t_lidar_user_data*)userdata;
		cv::Point center(user_data->image_width / 2, user_data->image_width / 2);
		for (int i = 0; i < LIDAR_NUM_STEPS; i++) {
			cv::Point old_p;
			old_p.x = -user_data->LIDAR_controller->lidar_distances[i] * user_data->lidar_map_scale_factor * sin(i / 100.0 * M_PI - M_PI / 2);
			old_p.y = -user_data->LIDAR_controller->lidar_distances[i] * user_data->lidar_map_scale_factor * cos(i / 100.0 * M_PI - M_PI / 2);
			cv::circle(*(user_data->lidar_image), center + old_p, 5, cv::Scalar(0, 0, 0), 1, 8);
		}
		char text[100];
		sprintf(text, "scale = %.2lf", user_data->lidar_map_scale_factor);
		int font_face = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
		cv::Point text_position(10, 25);

		cv::putText(*(user_data->lidar_image), text, text_position, font_face, 1, cv::Scalar::all(0), 1, 8);
		// delete the robot
		cv::rectangle(*user_data->lidar_image, cv::Point(center.x - 175 * user_data->lidar_map_scale_factor, center.y), cv::Point(center.x + user_data->lidar_map_scale_factor * 175, center.y + 600 * user_data->lidar_map_scale_factor), cv::Scalar(0, 0, 0));


		if (delta > 0)
			user_data->lidar_map_scale_factor += 0.01;
		else
			if (delta < 0)
				user_data->lidar_map_scale_factor -= 0.01;

		if (user_data->lidar_map_scale_factor < 0.01)
			user_data->lidar_map_scale_factor = 0.01;

		for (int i = 0; i < LIDAR_NUM_STEPS; i++) {
			// draw the new point
			Point new_p;
			new_p.x = -user_data->LIDAR_controller->lidar_distances[i] * user_data->lidar_map_scale_factor * sin(i / 100.0 * M_PI - M_PI / 2);
			new_p.y = -user_data->LIDAR_controller->lidar_distances[i] * user_data->lidar_map_scale_factor * cos(i / 100.0 * M_PI - M_PI / 2);
			circle(*(user_data->lidar_image), center + new_p, 5, Scalar(0, 255, 0), 1, 8);
		}

		// robot
		rectangle(*user_data->lidar_image, Point(center.x - 175 * user_data->lidar_map_scale_factor, center.y), Point(center.x + user_data->lidar_map_scale_factor * 175, center.y + 600 * user_data->lidar_map_scale_factor), Scalar(0, 255, 0));

		sprintf(text, "scale = %.2lf", user_data->lidar_map_scale_factor);
		cv::putText(*user_data->lidar_image, text, text_position, font_face, 1, Scalar::all(255), 1, 8);
	}
}
//----------------------------------------------------------------
bool update_lidar_image(t_lidar_controller &LIDAR_controller, Mat &lidar_image, int image_width, double lidar_map_scale_factor, f_log_callback to_log)
{
	int motor_position;
	intptr_t distance;
	Point center(image_width / 2, image_width / 2);
	bool at_least_one_new_LIDAR_distance = false;

	while (LIDAR_controller.arduino_controller.query_for_event(LIDAR_READ_EVENT, &motor_position, &distance)) {  // have we received the event from Serial ?

																						  // delete old distance
		Point old_p;
		old_p.x = -LIDAR_controller.lidar_distances[motor_position] * lidar_map_scale_factor * sin(motor_position / 100.0 * M_PI - M_PI / 2);
		old_p.y = -LIDAR_controller.lidar_distances[motor_position] * lidar_map_scale_factor * cos(motor_position / 100.0 * M_PI - M_PI / 2);
		circle(lidar_image, center + old_p, 5, Scalar(0, 0, 0), 1, 8);

	// draw the new point
		LIDAR_controller.lidar_distances[motor_position] = distance;
		Point new_p;
		new_p.x = -distance * lidar_map_scale_factor * sin(motor_position / 100.0 * M_PI - M_PI / 2);
		new_p.y = -distance * lidar_map_scale_factor * cos(motor_position / 100.0 * M_PI - M_PI / 2);
		circle(lidar_image, center + new_p, 5, Scalar(0, 255, 0), 1, 8);
		at_least_one_new_LIDAR_distance = true;
		if (to_log) {
			char tmp_s[100];
			sprintf(tmp_s, "Motor position = %d LIDAR distance = %d\n", motor_position, (int)distance);
			to_log(tmp_s);
		}
	}
	return at_least_one_new_LIDAR_distance;
}
//----------------------------------------------------------------
