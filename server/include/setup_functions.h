// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#ifndef setup_function_H
#define setup_function_H

#include "opencv2\objdetect\objdetect.hpp"

bool init_face_classifier(cv::CascadeClassifier &face_classifier, char* error_string);

#endif