// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
//--------------------------------------------

"use strict";
//--------------------------------------------
function send_capture_head_camera()
{
	send_command_to_robot(1 << 7, CAPTURE_HEAD_CAMERA);
	send_pause_robot();
	document.getElementById("img_camera").src = "head.jpg?t=" + Math.random();
}
//--------------------------------------------
function send_capture_left_arm_camera()
{
	send_command_to_robot(1 << 7, CAPTURE_LEFT_ARM_CAMERA);	
}
//--------------------------------------------------------------------
function send_capture_right_arm_camera()
{
	send_command_to_robot(1 << 7, CAPTURE_RIGHT_ARM_CAMERA);	
}
//--------------------------------------------------------------------
