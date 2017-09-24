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

	document.getElementById("body_button").disabled = state;
	document.getElementById("arm_button").disabled = state;
	document.getElementById("shoulder_button").disabled = state;
	document.getElementById("elbow_button").disabled = state;
	document.getElementById("forearm_button").disabled = state;
	document.getElementById("gripper_button").disabled = state;
	
	document.getElementById("rotate_head_button").disabled = state;

	document.getElementById("track_head_button").disabled = state;
	document.getElementById("follow_person_button").disabled = state;

	document.getElementById("capture_head_camera_button").disabled = state;
	document.getElementById("capture_left_arm_camera_button").disabled = state;
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

	document.getElementById("body_button").style.fontWeight = "normal";	
	document.getElementById("arm_button").style.fontWeight = "normal";	
	document.getElementById("shoulder_button").style.fontWeight = "normal";	
	document.getElementById("elbow_button").style.fontWeight = "normal";	
	document.getElementById("forearm_button").style.fontWeight = "normal";	
	document.getElementById("gripper_button").style.fontWeight = "normal";	
	
	document.getElementById("rotate_head_button").style.fontWeight = "normal";	

	document.getElementById("track_head_button").style.fontWeight = "normal";	
	document.getElementById("follow_person_button").style.fontWeight = "normal";	

	document.getElementById("capture_head_camera_button").style.fontWeight = "normal";	
	document.getElementById("capture_left_arm_camera_button").style.fontWeight = "normal";	
	
}
//----------------------------------------------------------------