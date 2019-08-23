// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------

#include "setup_functions.h"

//----------------------------------------------------------------
bool init_face_classifier(cv::CascadeClassifier &face_classifier, char* error_string)
{
	//-------------- START INITIALIZATION ------------------------------

	// create cascade for face reco
	  // load haarcascade library
	if (face_classifier.empty())
		if (!face_classifier.load("c:/jenny5/tests/jenny5-websocket-server/x64/Release/haarcascade_frontalface_alt.xml")) {
			sprintf(error_string, "Cannot load haarcascade! Please place the file in the correct folder!\n");
			return false;
		}

	return true;
}
//----------------------------------------------------------------