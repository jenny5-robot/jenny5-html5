// author: Mihai Oltean
// email: mihai.oltean@gmail.com
// main website: http://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot

// MIT License
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
#define LEFT_ARM_BODY_LEFT_RIGHT_COMMAND 7
#define LEFT_ARM_UP_DOWN_COMMAND 8
#define LEFT_ARM_ROTATE_COMMAND 9
#define LEFT_ARM_ELBOW_MOVE_COMMAND 10
#define LEFT_ARM_FOREARM_MOVE_COMMAND 11
#define LEFT_ARM_WRIST_MOVE_COMMAND 12
#define LEFT_ARM_GRIPPER_MOVE_COMMAND 13
#define LEFT_ARM_READ_SENSORS_COMMAND 14

#define LEFT_ARM_WAVE_COMMAND 21
#define POWER_DISABLE_COMMAND 22

#define HEAD_ROTATE_COMMAND 31

#define FACE_TRACKING_COMMAND 41
#define FOLLOW_PERSON_COMMAND 42

#define CAPTURE_HEAD_CAMERA 51
#define CAPTURE_LEFT_ARM_CAMERA 52

#define ERROR_PROCESSING 100
// ---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
	int process_command(unsigned char bx, unsigned char by);
	void print_message(FILE *f, char* message);
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