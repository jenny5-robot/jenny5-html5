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
