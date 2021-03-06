// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
//--------------------------------------------
"use strict";

var sensor_bbox_width;
var sensor_bbox_height;
var sensor_bbox_center;
//----------------------------------------------------------------
function on_load_page()
{
	set_disable_state(true);
	
	var svg = document.getElementById("svg_device_orientation");
	var document_width = document.body.clientWidth;
	svg.setAttribute("width", document_width);
	
	var sensor_bbox = document.getElementById("sensor_bbox");
	sensor_bbox.setAttribute("width", document_width);
	sensor_bbox_width = document_width;
	
	sensor_bbox_height = sensor_bbox.getAttribute("height");
	sensor_bbox_center = {x : sensor_bbox_width / 2, y : sensor_bbox_height / 2};
}
//----------------------------------------------------------------
function set_disable_state(state)
{
	document.getElementById("pause_robot_button").disabled = state;
	
	document.getElementById("disable_power_button").disabled = state;

	document.getElementById("navigate_platform_button").disabled = state;
	document.getElementById("rotate_platform_button").disabled = state;
	document.getElementById("leg_button").disabled = state;

	document.getElementById("body_button_left").disabled = state;
	document.getElementById("arm_button_left").disabled = state;
	document.getElementById("shoulder_button_left").disabled = state;
	document.getElementById("elbow_button_left").disabled = state;
	document.getElementById("forearm_button_left").disabled = state;
	document.getElementById("wrist_button_left").disabled = state;
	document.getElementById("id_wave_left_arm").disabled = state;
	document.getElementById("id_read_sensors_left_arm").disabled = state;
	
	document.getElementById("body_button_right").disabled = state;
	document.getElementById("arm_button_right").disabled = state;
	document.getElementById("shoulder_button_right").disabled = state;
	document.getElementById("elbow_button_right").disabled = state;
	document.getElementById("forearm_button_right").disabled = state;
	document.getElementById("wrist_button_right").disabled = state;
	document.getElementById("id_wave_right_arm").disabled = state;
	document.getElementById("id_read_sensors_right_arm").disabled = state;

	document.getElementById("rotate_head_button").disabled = state;

	document.getElementById("track_head_button").disabled = state;
	document.getElementById("follow_person_button").disabled = state;

	document.getElementById("capture_head_camera_button").disabled = state;
	document.getElementById("capture_left_arm_camera_button").disabled = state;
	document.getElementById("capture_right_arm_camera_button").disabled = state;
	document.getElementById("speak_button").style.color = "";
	document.getElementById("speak_button").disabled = state;
}
//----------------------------------------------------------------
function set_all_up()
{
	document.getElementById("pause_robot_button").style.fontWeight = "normal";
		
	document.getElementById("disable_power_button").style.fontWeight = "normal";

	document.getElementById("navigate_platform_button").style.fontWeight = "normal";
	document.getElementById("rotate_platform_button").style.fontWeight = "normal";
	document.getElementById("leg_button").style.fontWeight = "normal";

	document.getElementById("body_button_left").style.fontWeight = "normal";
	document.getElementById("arm_button_left").style.fontWeight = "normal";
	document.getElementById("shoulder_button_left").style.fontWeight = "normal";
	document.getElementById("elbow_button_left").style.fontWeight = "normal";
	document.getElementById("forearm_button_left").style.fontWeight = "normal";
	document.getElementById("wrist_button_left").style.fontWeight = "normal";
	document.getElementById("id_wave_left_arm").style.fontWeight = "normal";
	document.getElementById("id_read_sensors_left_arm").style.fontWeight = "normal";
	
	document.getElementById("body_button_right").style.fontWeight = "normal";
	document.getElementById("arm_button_right").style.fontWeight = "normal";
	document.getElementById("shoulder_button_right").style.fontWeight = "normal";
	document.getElementById("elbow_button_right").style.fontWeight = "normal";
	document.getElementById("forearm_button_right").style.fontWeight = "normal";
	document.getElementById("wrist_button_right").style.fontWeight = "normal";
	document.getElementById("id_wave_right_arm").style.fontWeight = "normal";
	document.getElementById("id_read_sensors_right_arm").style.fontWeight = "normal";

	document.getElementById("rotate_head_button").style.fontWeight = "normal";

	document.getElementById("track_head_button").style.fontWeight = "normal";
	document.getElementById("follow_person_button").style.fontWeight = "normal";

	document.getElementById("capture_head_camera_button").style.fontWeight = "normal";
	document.getElementById("capture_left_arm_camera_button").style.fontWeight = "normal";
	document.getElementById("capture_right_arm_camera_button").style.fontWeight = "normal";
	
}
//----------------------------------------------------------------