// author: Mihai Oltean
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
// ---------------------------------------------------------------------------

// MIT License
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "head_face_follow.h"
#include "setup_functions.h"
#include "jenny5_defs.h"


using namespace cv;

//----------------------------------------------------------------
int head_face_follow(t_head_controller *jenny5_head_controller, CascadeClassifier *face_classifier, f_log_callback to_log, f_stop_callback stop_function, char* window_name)
{
	// initialization

	Mat cam_frame; // images used in the proces
	Mat gray_frame;

	bool active = true;
	while (active) {        // starting infinit loop

		if (stop_function)
		  if (stop_function())
			break;

		if (!jenny5_head_controller->head_arduino_controller.update_commands_from_serial())
			Sleep(DOES_NOTHING_SLEEP); // no new data from serial ... we make a little pause so that we don't kill the processor

		jenny5_head_controller->cam >> cam_frame; // put captured-image frame in frame

		cvtColor(cam_frame, gray_frame, COLOR_BGR2GRAY); // convert to gray and equalize
		equalizeHist(gray_frame, gray_frame);

		std::vector<Rect> faces;// create an array to store the faces found

								// find and store the faces
		face_classifier->detectMultiScale(gray_frame, faces, 1.1, 3, /*HAAR_FIND_BIGGEST_OBJECT | HAAR_SCALE_IMAGE*/0, Size(50, 50));

		t_CENTER_POINT head_center;

		bool face_found = get_biggest_face(faces, head_center);

		if (face_found) {
			Point p1(head_center.x - head_center.range, head_center.y - head_center.range);
			Point p2(head_center.x + head_center.range, head_center.y + head_center.range);
			// draw an outline for the faces
			rectangle(cam_frame, p1, p2, Scalar(0, 255, 0, 0), 1, 8, 0);
		}
		else {
			Sleep(DOES_NOTHING_SLEEP); // no face found
									   //head_controller.send_move_motor(MOTOR_HEAD_HORIZONTAL, 0);// stops 
									   //head_controller.send_move_motor(MOTOR_HEAD_VERTICAL, 0);
		}

		if (window_name) {
			imshow(window_name, cam_frame);
			int key = waitKey(1);
			if (key == VK_ESCAPE)  // break the loop
				active = false;
		}

		if (jenny5_head_controller->head_arduino_controller.get_sonar_state(0) == COMMAND_DONE) {// I ping the sonar only if no ping was sent before
			jenny5_head_controller->head_arduino_controller.send_get_sonar_distance(0);
			jenny5_head_controller->head_arduino_controller.set_sonar_state(0, COMMAND_SENT);
			to_log("U0# - sent\n");
		}

		if (face_found) {// 
						 // horizontal movement motor

						 // send a command to the module so that the face is in the center of the image
			if (head_center.x < cam_frame.cols / 2 - CAM_PIXELS_TOLERANCE) {
				tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
				int num_steps_x = int(angle_offset.degrees_from_center_x / 1.8 * HEAD_MOTOR_GEAR_REDUCTION);

				jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_NECK, num_steps_x);
				jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_NECK, COMMAND_SENT);
				char tmp_s[100];
				sprintf(tmp_s, "M%d %d# - sent\n", HEAD_MOTOR_NECK, num_steps_x);
				to_log(tmp_s);

				//	head_controller.set_sonar_state(0, COMMAND_DONE); // if the motor has been moved the previous distances become invalid
			}
			else
				if (head_center.x > cam_frame.cols / 2 + CAM_PIXELS_TOLERANCE) {
					tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
					int num_steps_x = int(angle_offset.degrees_from_center_x / 1.8 * HEAD_MOTOR_GEAR_REDUCTION);

					jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_NECK, num_steps_x);
					jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_NECK, COMMAND_SENT);
					char tmp_s[100];
					sprintf(tmp_s, "M%d %d# - sent\n", HEAD_MOTOR_NECK, num_steps_x);
					to_log(tmp_s);

					//	head_controller.set_sonar_state(0, COMMAND_DONE); // if the motor has been moved the previous distances become invalid
				}
				else {
					// face is in the center, so I do not move
					Sleep(DOES_NOTHING_SLEEP);
				}

				// vertical movement motor
				// send a command to the module so that the face is in the center of the image
				if (head_center.y < cam_frame.rows / 2 - CAM_PIXELS_TOLERANCE) {
					tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
					int num_steps_y = int(angle_offset.degrees_from_center_y / 1.8 * HEAD_MOTOR_GEAR_REDUCTION);

					jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, num_steps_y);
					jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_FACE, COMMAND_SENT);
					char tmp_s[100];
					sprintf(tmp_s, "M%d %d# - sent\n", HEAD_MOTOR_FACE, num_steps_y);
					to_log(tmp_s);
					//	head_controller.set_sonar_state(0, COMMAND_DONE); // if the motor has been moved the previous distances become invalid
				}
				else
					if (head_center.y > cam_frame.rows / 2 + CAM_PIXELS_TOLERANCE) {
						tracking_data angle_offset = get_offset_angles(920, Point(head_center.x, head_center.y));
						int num_steps_y = int(angle_offset.degrees_from_center_y / 1.8 * HEAD_MOTOR_GEAR_REDUCTION);

						jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, num_steps_y);
						jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_FACE, COMMAND_SENT);
						char tmp_s[100];
						sprintf(tmp_s, "M%d -%d# - sent\n", HEAD_MOTOR_FACE, num_steps_y);
						to_log(tmp_s);
						//		head_controller.set_sonar_state(0, COMMAND_DONE); // if the motor has been moved the previous distances become invalid
					}

		}

		// now extract the executed moves from the queue ... otherwise they will just stay there
		if (jenny5_head_controller->head_arduino_controller.get_stepper_motor_state(HEAD_MOTOR_NECK) == COMMAND_SENT) {// if a command has been sent
			if (jenny5_head_controller->head_arduino_controller.query_for_event(STEPPER_MOTOR_MOVE_DONE_EVENT, HEAD_MOTOR_NECK)) { // have we received the event from Serial ?
				jenny5_head_controller->head_arduino_controller.set_stepper_motor_state(HEAD_MOTOR_FACE, COMMAND_DONE);
				char tmp_s[100];
				sprintf(tmp_s, "M%d# - done\n", HEAD_MOTOR_FACE);
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

		// read to see if there is any distance received from sonar
		if (jenny5_head_controller->head_arduino_controller.get_sonar_state(0) == COMMAND_SENT) {// if a command has been sent
			intptr_t distance;
			if (jenny5_head_controller->head_arduino_controller.query_for_event(SONAR_EVENT, 0, &distance)) { // have we received the event from Serial ?
				jenny5_head_controller->head_arduino_controller.set_sonar_state(0, COMMAND_DONE);
				char tmp_s[100];
				sprintf(tmp_s, "distance = %Ix cm\n", distance);
				to_log(tmp_s);
			}
		}
		
	}

	jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_NECK, 0);
	jenny5_head_controller->head_arduino_controller.send_move_stepper_motor(HEAD_MOTOR_FACE, 0);

	return 0;
}
//----------------------------------------------------------------