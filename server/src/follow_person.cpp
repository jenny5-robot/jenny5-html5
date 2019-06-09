// author: Mihai Oltean
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------
#define _USE_MATH_DEFINES

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>


#include "follow_person.h"
#include "setup_functions.h"

#include "lidar_map.h"
#include "lidar_controller.h"
#include "platform_controller.h"

#include "jenny5_defs.h"


#define DC_MOTOR_SPEED 5000
#define DC_MOTOR_SPEED_ROTATE 5000


using namespace cv;
//----------------------------------------------------------------
bool clear_ahead(int *lidar_distances)
{
	for (int i = 40; i < 60; i++)
		if (lidar_distances[i] < 800)
			return false;
	return true;
}
//----------------------------------------------------------------
int follow_person(t_head_controller *jenny5_head_controller, t_lidar_controller *LIDAR_controller, t_platform_controller * platform_controller, cv::CascadeClassifier *face_classifier, f_log_callback to_log, f_stop_callback stop_function, char* head_window_name, char* LIDAR_window_name)
{
	LIDAR_controller->arduino_controller.send_set_LiDAR_motor_speed_and_acceleration(30, 100);
	LIDAR_controller->arduino_controller.send_LiDAR_go();

	int image_width = 600;
	Point center(image_width / 2, image_width / 2);
	Mat lidar_map_image = Mat::zeros(image_width, image_width, CV_8UC3); //Mat lidar_map_image;

	t_lidar_user_data lidar_user_data;
	lidar_user_data.lidar_image = &lidar_map_image;
	lidar_user_data.image_width = image_width;
	lidar_user_data.LIDAR_controller = LIDAR_controller;
	for (int i = 0; i < LIDAR_NUM_STEPS; i++)
		LIDAR_controller->lidar_distances[i] = 0;
	lidar_user_data.lidar_map_scale_factor = 0.1;

	create_and_init_lidar_image(lidar_map_image, image_width, lidar_user_data.lidar_map_scale_factor);

	if (LIDAR_window_name)
		setMouseCallback(LIDAR_window_name, on_lidar_mouse_event, &lidar_user_data);

	Mat head_cam_frame; // images used in the proces
	Mat gray_frame;

//	double scale_factor;

	bool active = true;
	while (active){        // starting infinit loop

		if (stop_function)
			if (stop_function())
				break;

		if (!jenny5_head_controller->head_arduino_controller.update_commands_from_serial() && !LIDAR_controller->arduino_controller.update_commands_from_serial())
			Sleep(DOES_NOTHING_SLEEP); // no new data from serial ... we take a little break so that we don't kill the processor
		else {

			// extract all data from LIDAR 
			bool at_least_one_new_LIDAR_distance = update_lidar_image(*LIDAR_controller, lidar_map_image, image_width, lidar_user_data.lidar_map_scale_factor, to_log);

			if (at_least_one_new_LIDAR_distance)
				if(LIDAR_window_name)
					imshow(LIDAR_window_name, lidar_map_image);

		}
		jenny5_head_controller->cam >> head_cam_frame; // put captured-image frame in frame

		cvtColor(head_cam_frame, gray_frame, COLOR_BGR2GRAY); // convert to gray and equalize
		equalizeHist(gray_frame, gray_frame);

		std::vector<Rect> faces;// create an array to store the found faces

								// find and store the faces
		face_classifier->detectMultiScale(gray_frame, faces, 1.1, 3, /*CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE*/0, Size(30, 30));

		t_CENTER_POINT head_center;

		bool face_found = get_biggest_face(faces, head_center);

		if (face_found) {
			Point p1(head_center.x - head_center.range, head_center.y - head_center.range);
			Point p2(head_center.x + head_center.range, head_center.y + head_center.range);
			// draw an outline for the faces
			rectangle(head_cam_frame, p1, p2, Scalar(0, 255, 0, 0), 1, 8, 0);
		}
		else {
			Sleep(DOES_NOTHING_SLEEP); // no face found
									   // I have to move the head right-left; top-down
		}

		if (head_window_name) {
			imshow(head_window_name, head_cam_frame); // display the result
			int key = waitKey(1);
			if (key == VK_ESCAPE)  // break the loop
				active = false;
		}

		if (face_found) {
			// send a command to the module so that the face is in the center of the image
			if (head_center.x > head_cam_frame.cols / 2 + CAM_PIXELS_TOLERANCE) {

				// rotate
				platform_controller->move_left_motor(DC_MOTOR_SPEED_ROTATE, 1);
				platform_controller->move_right_motor(-DC_MOTOR_SPEED_ROTATE, 1);
				char tmp_s[100];
				sprintf(tmp_s, "rotate right - sent\n");
				to_log(tmp_s);
			}
			else
				if (head_center.x < head_cam_frame.cols / 2 - CAM_PIXELS_TOLERANCE) {

					//					tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
					//					int num_steps_x = (int)(angle_offset.degrees_from_center_x / 1.8 * 8) * TRACKS_MOTOR_REDUCTION;

					// rotate
					platform_controller->move_left_motor(-DC_MOTOR_SPEED_ROTATE, 1);
					platform_controller->move_right_motor(DC_MOTOR_SPEED_ROTATE, 1);
					char tmp_s[100];
					sprintf(tmp_s, "rotate left - sent\n");
					to_log(tmp_s);
				}
				else {

					// face is in the center, so I move equaly with both motors
					if (head_center.range < HEAD_RADIUS_TO_REVERT - HEAD_RADIUS_TOLERANCE) {
						// move forward
						// only if LIDAR distance to the front point is very far from the robot
					//	if (clear_ahead(lidar_user_data.LIDAR_controller->lidar_distances)) {
							platform_controller->move_left_motor(DC_MOTOR_SPEED, 1);
							platform_controller->move_right_motor(DC_MOTOR_SPEED, 1);

							char tmp_s[100];
							sprintf(tmp_s, "move ahead - sent\n");
							to_log(tmp_s);
					/*	
					}
						else {
							// stop the robot -- here I have to move around the obstacle
							platform_controller.stop_motors();
							char tmp_s[100];
							sprintf(tmp_s, "cannot move ahead - obstacle detected\n");
							to_log(tmp_s);
						}
						*/
					}
					else 
						if (head_center.range > HEAD_RADIUS_TO_REVERT + HEAD_RADIUS_TOLERANCE) {
						// move backward
						platform_controller->move_left_motor(-DC_MOTOR_SPEED, 1);
						platform_controller->move_right_motor(-DC_MOTOR_SPEED, 1);
						char tmp_s[100];
						sprintf(tmp_s, "move backward - sent\n");
						to_log(tmp_s);
					}
						else {
							platform_controller->stop_motors();
							char tmp_s[100];
							sprintf(tmp_s, "dead zone - stop platform motors sent\n");
							to_log(tmp_s);
						}
				}

				// vertical movement motor
				// send a command to the module so that the face is in the center of the image
				if (head_center.y < head_cam_frame.rows / 2 - CAM_PIXELS_TOLERANCE) {
					tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
					int num_steps_y = int(angle_offset.degrees_from_center_y / 1.8 * 27.0);

					jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, num_steps_y);
					jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_FACE, COMMAND_SENT);
					char tmp_s[100];
					sprintf(tmp_s, "move head down M%d %d# - sent\n", HEAD_MOTOR_FACE, num_steps_y);
					to_log(tmp_s);
					//	head_controller.set_sonar_state(0, COMMAND_DONE); // if the motor has been moved the previous distances become invalid
				}
				else
					if (head_center.y > head_cam_frame.rows / 2 + CAM_PIXELS_TOLERANCE) {
						tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
						int num_steps_y = int(angle_offset.degrees_from_center_y / 1.8 * 27.0);

						jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, num_steps_y);
						jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_FACE, COMMAND_SENT);
						char tmp_s[100];
						sprintf(tmp_s, "hove head up M%d -%d# - sent\n", HEAD_MOTOR_FACE, num_steps_y);
						to_log(tmp_s);
						//		head_controller.set_sonar_state(0, COMMAND_DONE); // if the motor has been moved the previous distances become invalid
					}
		}
		else {
			// no face found ... so stop the platoform motors
			platform_controller->stop_motors();

			char tmp_s[100];
			sprintf(tmp_s, "no face found - motors stopped\n");
			to_log(tmp_s);
		}

		// now extract the executed moves from the queue ... otherwise they will just sit there and will occupy memory
		if (jenny5_head_controller->head_arduino_controller.get_stepper_motor_state(HEAD_MOTOR_NECK) == COMMAND_SENT) {// if a command has been sent
			if (jenny5_head_controller->head_arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, HEAD_MOTOR_NECK)) { // have we received the event from Serial ?
				jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_NECK, COMMAND_DONE);
				char tmp_s[100];
				sprintf(tmp_s, "M%d# - done\n", HEAD_MOTOR_NECK);
				to_log(tmp_s);
			}
		}

		// now extract the moves done from the queue
		if (jenny5_head_controller->head_arduino_controller.get_stepper_motor_state(HEAD_MOTOR_FACE) == COMMAND_SENT) {// if a command has been sent
			if (jenny5_head_controller->head_arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, HEAD_MOTOR_FACE)) { // have we received the event from Serial ?
				jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_FACE, COMMAND_DONE);
				char tmp_s[100];
				sprintf(tmp_s, "M%d# - done\n", HEAD_MOTOR_FACE);
				to_log(tmp_s);
			}
		}
	}

	// stops all motors
	jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, 0);
	jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_NECK, 0);

	jenny5_head_controller->head_arduino_controller.send_disable_stepper_motor(HEAD_MOTOR_FACE);
	jenny5_head_controller->head_arduino_controller.send_disable_stepper_motor(HEAD_MOTOR_NECK);

	platform_controller->stop_motors();
	
	LIDAR_controller->arduino_controller.send_LiDAR_stop();

	return true;
}
//----------------------------------------------------------------