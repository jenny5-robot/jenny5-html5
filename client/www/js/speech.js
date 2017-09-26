//----------------------------------------------------------------
var SpeechRecognition = SpeechRecognition || webkitSpeechRecognition
var recognition = new SpeechRecognition();
recognition.lang = 'en-US';
recognition.interimResults = false;
recognition.maxAlternatives = 1;

recognition.onresult = on_speech_recognition_result;
recognition.onnomatch = on_speech_no_match;
recognition.onerror = on_speech_recognition_error;
recognition.onspeechend = on_speech_end;
recognition.onend = on_end;

//----------------------------------------------------------------

var speech_synth = window.speechSynthesis;
var utterThis = new SpeechSynthesisUtterance();

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
		case "head rotate":
			utterThis.text = "head rotate";
			send_rotate_head();
			break;
		case "picture":
			utterThis.text = "picture";
			send_capture_head_camera();
			break;
		case "body":
			utterThis.text = "body";
			send_body_motor_move();
			break;
		case "arm":
			utterThis.text = "arm";
			send_arm_motor_move();
			break;
		case "shoulder":
			utterThis.text = "shoulder";
			send_shoulder_motor_move();
			break;
		case "elbow":
			utterThis.text = "elbow";
			send_elbow_motor_move();
			break;
		case "forearm":
			utterThis.text = "forearm";
			send_forearm_motor_move();
			break;
		case "gripper":
			utterThis.text = "gripper";
			send_gripper_motor_move();
			break;			
			
		default:
			utterThis.text = text + "not recognized.";
			
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
function on_end()
{
	var button_speak = document.getElementById("speak_button");
	var color = button_speak.style.color;
	
	if (color == "red"){
		speech_synth.speak(utterThis);
		recognition.start();
		utterThis.text = "";
	}
}
//----------------------------------------------------------------