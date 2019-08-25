// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
//------------------------------------------------------------------

#ifndef SETTINGS_H
#define SETTING_H

#define STEP_SPEED_ARM 10

#define ARM_dead_zone_length 12

#define ARM_MOTOR_NUM_STEPS 15000

#define LEG_BOTTOM_DIRECTION -1
#define LEG_TOP_DIRECTION -1

#define PLATFORM_LEFT_MOTOR_DIRECTION -1
#define PLATFORM_RIGHT_MOTOR_DIRECTION -1

#define PAUSED_STATE 0
#define PLATFORM_NAVIGATE_STATE 1
#define PLATFORM_ROTATE_STATE 2
#define LEG_MOVE_STATE 3

#define LEFT_ARM_BODY_LEFT_RIGHT_MOVE_STATE 40
#define LEFT_ARM_UP_DOWN_MOVE_STATE 41
#define LEFT_ARM_ROTATE_STATE 42
#define LEFT_ARM_ELBOW_MOVE_STATE 43
#define LEFT_ARM_FOREARM_MOVE_STATE 44
#define LEFT_ARM_WRIST_MOVE_STATE 45
#define LEFT_ARM_GRIPPER_MOVE_STATE 46
#define WAVE_LEFT_ARM_STATE 48

#define RIGHT_ARM_BODY_LEFT_RIGHT_MOVE_STATE 50
#define RIGHT_ARM_UP_DOWN_MOVE_STATE 51
#define RIGHT_ARM_ROTATE_STATE 52
#define RIGHT_ARM_ELBOW_MOVE_STATE 53
#define RIGHT_ARM_FOREARM_MOVE_STATE 54
#define RIGHT_ARM_WRIST_MOVE_STATE 55
#define RIGHT_ARM_GRIPPER_MOVE_STATE 56
#define WAVE_RIGHT_ARM_STATE 58


#define HEAD_ROTATE_STATE 8

#define FACE_TRACKING_STATE 9
#define FOLLOW_PERSON_STATE 10

#endif