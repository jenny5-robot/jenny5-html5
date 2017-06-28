"use strict";
//--------------------------------------------
function send_capture_head_camera()
{
	send_command_to_robot(1 << 7, CAPTURE_HEAD_CAMERA);
	document.getElementById("img_camera").src = "head.jpg?t=" + Math.random();
}
//--------------------------------------------