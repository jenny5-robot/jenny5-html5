// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

#define _USE_MATH_DEFINES

#include "utils.h"
#include <time.h>

//----------------------------------------------------------------
bool get_biggest_face(std::vector<cv::Rect> faces, t_CENTER_POINT &center)
{

	center.x = -1;
	center.x = -1;
	center.range = 0;

	bool found_one = false;
	for (unsigned int i = 0; i < faces.size(); i++) {
		if ((faces[i].width) / 2 > center.range) {
			center.range = (faces[i].width) / 2;
			center.x = faces[i].x + center.range;
			center.y = faces[i].y + center.range;
			found_one = true;
		}
	}
	return found_one;
}
//----------------------------------------------------------------
tracking_data get_offset_angles(int webcam_model_number, double image_ratio, int image_width, int image_height, cv::Point position)
{
	tracking_data deviation;

	if (webcam_model_number == 910){
		if (image_ratio == 4 / 3.0){
			deviation.degrees_from_center_x = determine_offset_angle(position.x, C910_4_3_HORIZONTAL_FIELD_OF_VIEW, image_width);
			deviation.degrees_from_center_y = determine_offset_angle(position.y, C910_4_3_VERTICAL_FIELD_OF_VIEW, image_height);
		}
		else {
			deviation.degrees_from_center_x = determine_offset_angle(position.x, C910_16_9_HORIZONTAL_FIELD_OF_VIEW, image_width);
			deviation.degrees_from_center_y = determine_offset_angle(position.y, C910_16_9_VERTICAL_FIELD_OF_VIEW, image_height);
		}
	}
	else 
		if (webcam_model_number == 920){
		if (fabs(image_ratio - 4 / 3.0) < 1e-6){
			deviation.degrees_from_center_x = determine_offset_angle(position.x, C920_4_3_HORIZONTAL_FIELD_OF_VIEW, image_width);
			deviation.degrees_from_center_y = determine_offset_angle(position.y, C920_4_3_VERTICAL_FIELD_OF_VIEW, image_height);
		}
		else {
			deviation.degrees_from_center_x = determine_offset_angle(position.x, C920_16_9_HORIZONTAL_FIELD_OF_VIEW, image_width);
			deviation.degrees_from_center_y = determine_offset_angle(position.y, C920_16_9_VERTICAL_FIELD_OF_VIEW, image_height);
		}
	}

	return deviation;
}
//-----------------------------------------------------------------------
tracking_data get_offset_angles(int webcam_model_number, cv::Point position)
{
	tracking_data deviation;

	if (webcam_model_number == 910){
		deviation.degrees_from_center_x = determine_offset_angle(position.x, C910_4_3_HORIZONTAL_FIELD_OF_VIEW, 640);
		deviation.degrees_from_center_y = determine_offset_angle(position.y, C910_4_3_VERTICAL_FIELD_OF_VIEW, 480);
	}
	else
		if (webcam_model_number == 920){
			deviation.degrees_from_center_x = determine_offset_angle(position.x, C920_4_3_HORIZONTAL_FIELD_OF_VIEW, 640);
			deviation.degrees_from_center_y = determine_offset_angle(position.y, C920_4_3_VERTICAL_FIELD_OF_VIEW, 480);
		}

	return deviation;
}
//-----------------------------------------------------------------------
double determine_offset_angle(int position, double field_of_view, int number_of_pixels)
{
	double pixel_one_percent = (double)number_of_pixels / 100.0;
	double fov_one_percent = (double)field_of_view / 100.0;
	double offset_from_center = (position - number_of_pixels / 2.0) / pixel_one_percent;

	return offset_from_center * fov_one_percent;
}
//-----------------------------------------------------------------------
char* current_time_to_string(void)
{
	char *result = new char[100];
	strcpy(result, "");

	time_t _current_time = time(NULL);

	struct tm * ptm;

	ptm = localtime(&_current_time);

	strftime(result, 100, "%Y/%m/%d %H:%M:%S", ptm);

	return result;
}
//---------------------------------------------------------------------------