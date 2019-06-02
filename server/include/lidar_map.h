// author: Mihai Oltean
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#ifndef lidar_map_H
#define lidar_map_H

#include "jenny5_arduino_controller.h"

#include "lidar_controller.h"

#include <opencv2\objdetect\objdetect.hpp>
//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\imgproc\imgproc.hpp>

struct t_lidar_user_data
{
	cv::Mat *lidar_image;
	int image_width;
	double lidar_map_scale_factor;
	t_lidar_controller* LIDAR_controller;
};

void create_and_init_lidar_image(cv::Mat &lidar_image, int image_width, double lidar_map_scale_factor);
int lidar_map(t_lidar_controller &LIDAR_controller, const char* lidar_com_port, f_log_callback to_log);
void on_lidar_mouse_event(int event, int x, int y, int flags, void *userdata);
bool update_lidar_image(t_lidar_controller &LIDAR_controller, cv::Mat &lidar_image, int image_width, double lidar_map_scale_factor, f_log_callback to_log);

#endif