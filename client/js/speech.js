// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
//--------------------------------------------------------------
var SpeechRecognition = SpeechRecognition || webkitSpeechRecognition
var recognition = new SpeechRecognition();
recognition.lang = 'en-US';
recognition.interimResults = false;
recognition.maxAlternatives = 1;

recognition.onresult = on_speech_recognition_result;
recognition.onnomatch = on_speech_no_match;
recognition.onerror = on_speech_recognition_error;
recognition.onspeechend = on_speech_end;
recognition.onend = on_recognition_end;

//----------------------------------------------------------------

var speech_synth = window.speechSynthesis;
var utterThis = new SpeechSynthesisUtterance();
utterThis.onend = on_synthesis_end;

//----------------------------------------------------------------
function on_speak_button_clicked()
{
	var button_speak = document.getElementById("speak_button");
	var color = button_speak.style.color;
	
	if (color == ""){
		button_speak.style.color = "red";
		recognition.start();
	}
	else{
		button_speak.style.color = "";
		recognition.stop();
	}
}
//----------------------------------------------------------------
function stop_speech()
{
	document.getElementById("speak_button").style.color = "";
	recognition.stop();
}
//----------------------------------------------------------------
function on_speech_recognition_result(event)
{
	var last = event.results.length - 1;
	if (last < 0)
		return; // no word
	
	var text = event.results[last][0].transcript;
	
	switch (text){
		case "stop":
			utterThis.text = "robot stopped";
			send_pause_robot();
			break;
		case "navigate":
			utterThis.text = "navigate";
			send_start_platform_navigate();
			break;
		case "rotate":
			utterThis.text = "platform rotate";
			send_rotate_platform();
			break;
		case "disable":
			utterThis.text = "power disabled";
			send_disable_power();
			break;
		case "leg":
			utterThis.text = "leg move";
			send_leg_move();
			break;
		case "head":
			utterThis.text = "head rotate";
			send_rotate_head();
			break;
		case "picture":
			utterThis.text = "picture";
			send_capture_head_camera();
			break;
			
		case "left body":
			utterThis.text = "left body";
			send_left_body_motor_move();
			break;
		case "left arm":
			utterThis.text = "left arm";
			send_left_arm_motor_move();
			break;
		case "left shoulder":
			utterThis.text = "left shoulder";
			send_left_shoulder_motor_move();
			break;
		case "left elbow":
			utterThis.text = "left elbow";
			send_left_elbow_motor_move();
			break;
		case "left forearm":
			utterThis.text = "left forearm";
			send_left_forearm_motor_move();
			break;
		case "left wrist":
			utterThis.text = "left wrist";
			send_left_wrist_motor_move();
			break;
		case "left gripper":
			utterThis.text = "left gripper";
			send_left_gripper_motor_move();
			break;
		case "wave left hand":
			utterThis.text = "waving left hand";
			send_wave_left_arm();
			break;
		case "read left sensors":
			utterThis.text = "read left sensors";
			send_read_sensors_left_arm();
			break;

		case "right body":
			utterThis.text = "right body";
			send_right_body_motor_move();
			break;
		case "right arm":
			utterThis.text = "right arm";
			send_right_arm_motor_move();
			break;
		case "right shoulder":
			utterThis.text = "right shoulder";
			send_right_shoulder_motor_move();
			break;
		case "right elbow":
			utterThis.text = "right elbow";
			send_right_elbow_motor_move();
			break;
		case "right forearm":
			utterThis.text = "right forearm";
			send_right_forearm_motor_move();
			break;
		case "right wrist":
			utterThis.text = "right wrist";
			send_right_wrist_motor_move();
			break;
		case "right gripper":
			utterThis.text = "right gripper";
			send_right_gripper_motor_move();
			break;
		case "wave right hand":
			utterThis.text = "waving right hand";
			send_wave_right_arm();
			break;
		case "read right sensors":
			utterThis.text = "read right sensors";
			send_read_sensors_right_arm();
			break;
			
		case "Hello":
			utterThis.text = "Hello God!";			
			break;
		default:
			utterThis.text = "What is " + text + "?";
			
	}
}
//----------------------------------------------------------------
function on_speech_no_match(event) 
{

}
//----------------------------------------------------------------
function on_speech_recognition_error(event) 
{
	if (event.error != "no-speech"){
		recognition.stop();
		var local_utter = new SpeechSynthesisUtterance("Error " + event.error);
		speech_synth.speak(local_utter);
		document.getElementById("speak_button").style.color = "";
	}
}
//----------------------------------------------------------------
function on_speech_end()
{
	recognition.stop();
}
//----------------------------------------------------------------
function on_recognition_end()
{
	var button_speak = document.getElementById("speak_button");
	var color = button_speak.style.color;
	
	if (color == "red"){
		speech_synth.speak(utterThis);
	}
}
//----------------------------------------------------------------
function on_synthesis_end()
{
	var button_speak = document.getElementById("speak_button");
	var color = button_speak.style.color;

	if (color == "red"){
		recognition.start();
		utterThis.text = "";
	}
}