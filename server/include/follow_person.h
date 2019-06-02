// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#ifndef follow_person_H
#define follow_person_H

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "utils.h"
#include "head_controller.h"
#include "lidar_controller.h"
#include "platform_controller.h"


int follow_person(t_head_controller *jenny5_head_controller, t_lidar_controller *LIDAR_controller, t_platform_controller * platform_controller, cv::CascadeClassifier *face_classifier, f_log_callback to_log, f_stop_callback stop_function, char* head_window_name, char* LIDAR_window_name);


#endif