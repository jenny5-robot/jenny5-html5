// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------

#ifndef head_face_follow_H
#define head_face_follow_H

#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2\highgui\highgui.hpp"

#include "utils.h"
#include "head_controller.h"

int head_face_follow(t_head_controller *jenny5_head_controller, cv::CascadeClassifier *face_classifier, f_log_callback to_log, f_stop_callback stop_function, char* window_name);

#endif