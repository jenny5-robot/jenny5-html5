// author: Mihai Oltean, 
// email: mihai.oltean@gmail.com
// main website: https://www.jenny5.org
// mirror website: https://jenny5-robot.github.io
// source code: https://github.com/jenny5-robot


#ifndef SETTINGS_H
#define SETTING_H

#define STEP_SPEED_ARM 10

#define ARM_dead_zone_length 12

#define ARM_MOTOR_NUM_STEPS 15000

#define PLATFORM_COM_PORT "COM23"
#define LEG_COM_PORT "COM5"
#define LEFT_ARM_COM_PORT "COM3"
#define HEAD_COM_PORT "COM4"
#define LIDAR_COM_PORT "COM22"

#define LEG_BOTTOM_DIRECTION -1
#define LEG_TOP_DIRECTION -1

#define PLATFORM_LEFT_MOTOR_DIRECTION -1
#define PLATFORM_RIGHT_MOTOR_DIRECTION -1

#define PAUSED_STATE 0
#define PLATFORM_NAVIGATE_STATE 1
#define PLATFORM_ROTATE_STATE 2
#define LEG_MOVE_STATE 3
#define LEFT_ARM_BODY_LEFT_RIGHT_MOVE_STATE 4
#define LEFT_ARM_UP_DOWN_MOVE_STATE 5
#define LEFT_ARM_ROTATE_STATE 6
#define LEFT_ARM_ELBOW_MOVE_STATE 7
#define LEFT_ARM_FOREARM_MOVE_STATE 8
#define LEFT_ARM_WRIST_MOVE_STATE 9
#define LEFT_ARM_GRIPPER_MOVE_STATE 10

#define HEAD_ROTATE_STATE 20

#define FACE_TRACKING_STATE 31
#define FOLLOW_PERSON_STATE 32
#define WAVE_LEFT_ARM_STATE 33

#endif