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
function on_speech_recognition_result(event)
{
	var last = event.results.length - 1;
	var text = event.results[last][0].transcript;
	
	switch (text){
		case "stop":
			var utterThis = new SpeechSynthesisUtterance("robot stopped");
			speech_synth.speak(utterThis);
			send_pause_robot();
		break;
		case "navigate":
			var utterThis = new SpeechSynthesisUtterance("navigate");
			speech_synth.speak(utterThis);
			send_start_platform_navigate();
		break;
		case "rotate":
			var utterThis = new SpeechSynthesisUtterance("platform rotate");
			speech_synth.speak(utterThis);
			send_rotate_platform();
			break;
		case "disable":
			var utterThis = new SpeechSynthesisUtterance("power disabled");
			speech_synth.speak(utterThis);
			send_disable_power();
			break;
		case "leg":
			var utterThis = new SpeechSynthesisUtterance("leg move");
			speech_synth.speak(utterThis);
			send_leg_move();
			break;
		case "head rotate":
			var utterThis = new SpeechSynthesisUtterance("head rotate");
			speech_synth.speak(utterThis);
			send_rotate_head();
			break;
		case "picture":
			var utterThis = new SpeechSynthesisUtterance("picture");
			speech_synth.speak(utterThis);
			send_capture_head_camera();
			break;
		case "body":
			var utterThis = new SpeechSynthesisUtterance("body");
			speech_synth.speak(utterThis);
			send_body_motor_move();
			break;
		case "arm":
			var utterThis = new SpeechSynthesisUtterance("arm");
			speech_synth.speak(utterThis);
			send_arm_motor_move();
			break;
		case "shoulder":
			var utterThis = new SpeechSynthesisUtterance("shoulder");
			speech_synth.speak(utterThis);
			send_shoulder_motor_move();
			break;
		case "elbow":
			var utterThis = new SpeechSynthesisUtterance("elbow");
			speech_synth.speak(utterThis);
			send_elbow_motor_move();
			break;
		case "forearm":
			ar utterThis = new SpeechSynthesisUtterance("forearm");
			speech_synth.speak(utterThis);
			send_forearm_motor_move();
			break;
		case "gripper":
			var utterThis = new SpeechSynthesisUtterance("gripper");
			speech_synth.speak(utterThis);
			send_gripper_motor_move();
			break;			
			
		default:
			var utterThis = new SpeechSynthesisUtterance("Not recognized. Please repeat!");
			speech_synth.speak(utterThis);
	}
	
	//recognition.start();
}
//----------------------------------------------------------------
function on_speech_no_match(event) 
{
	var utterThis = new SpeechSynthesisUtterance("No match. Please repeat!");
	speech_synth.speak(utterThis);
	
	recognition.start();
}
//----------------------------------------------------------------
function on_speech_recognition_error(event) 
{
	var utterThis = new SpeechSynthesisUtterance("Error " + event.error);
	speech_synth.speak(utterThis);
	document.getElementById("speak_button").style.color = "";

	//diagnostic.textContent = 'Error occurred in recognition: ' + event.error;
  recognition.stop();
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
		recognition.start();
	}
}
//----------------------------------------------------------------