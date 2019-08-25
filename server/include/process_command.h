// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
// ---------------------------------------------------------------------------

#ifndef process_command_H
#define process_command_H

#include <stdio.h>
#include "jenny5_defs.h"

#define CONNECT_TO_ROBOT  1
#define DISCONNECT_FROM_ROBOT  2
#define PAUSE_PLATFORM_COMMAND 3
#define NAVIGATE_COMMAND 4
#define ROTATE_COMMAND 5
#define LEG_MOVE_COMMAND 6
#define POWER_DISABLE_COMMAND 7

#define HEAD_ROTATE_COMMAND 8

#define FACE_TRACKING_COMMAND 9
#define FOLLOW_PERSON_COMMAND 10

#define CAPTURE_HEAD_CAMERA 11
#define CAPTURE_LEFT_ARM_CAMERA 12
#define CAPTURE_RIGHT_ARM_CAMERA 13

#define LEFT_ARM_BODY_LEFT_RIGHT_COMMAND 40
#define LEFT_ARM_UP_DOWN_COMMAND 41
#define LEFT_ARM_ROTATE_COMMAND 42
#define LEFT_ARM_ELBOW_MOVE_COMMAND 43
#define LEFT_ARM_FOREARM_MOVE_COMMAND 44
#define LEFT_ARM_WRIST_MOVE_COMMAND 45
#define LEFT_ARM_GRIPPER_MOVE_COMMAND 46
#define LEFT_ARM_READ_SENSORS_COMMAND 47
#define LEFT_ARM_WAVE_COMMAND 48

#define RIGHT_ARM_BODY_LEFT_RIGHT_COMMAND 50
#define RIGHT_ARM_UP_DOWN_COMMAND 51
#define RIGHT_ARM_ROTATE_COMMAND 52
#define RIGHT_ARM_ELBOW_MOVE_COMMAND 53
#define RIGHT_ARM_FOREARM_MOVE_COMMAND 54
#define RIGHT_ARM_WRIST_MOVE_COMMAND 55
#define RIGHT_ARM_GRIPPER_MOVE_COMMAND 56
#define RIGHT_ARM_READ_SENSORS_COMMAND 57
#define RIGHT_ARM_WAVE_COMMAND 58

#define ERROR_PROCESSING 100
// ---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
	int process_command(unsigned char bx, unsigned char by);
	void print_message(FILE *f, char* message);
	void print_const_message(FILE* f, const char* message);

	void current_time_to_string(char *str_result);
	void stop_robot(void);
	void disconnect_robot(void);
	void init_robot(void);
	void update_commands_from_serial(void);
	int is_disconected(void);

	extern FILE *f_log;
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif