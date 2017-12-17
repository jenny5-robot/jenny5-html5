// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
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