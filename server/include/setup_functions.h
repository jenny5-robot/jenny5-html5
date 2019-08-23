// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------

#ifndef setup_function_H
#define setup_function_H

#include "opencv2\objdetect\objdetect.hpp"

bool init_face_classifier(cv::CascadeClassifier &face_classifier, char* error_string);

#endif