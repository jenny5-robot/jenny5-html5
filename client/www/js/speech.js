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
//----------------------------------------------------------------


var speech_synth = window.speechSynthesis;

//----------------------------------------------------------------
function enable_speak()
{
	var button_speak = document.getElementById("speak_button");
	var color = button_speak.style.color;
	
	if (color == "black"){
		button_speak.style.color = "red";
		recognition.start();
	}
	else{
		button_speak.style.color = "black";
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
		default:
			var utterThis = new SpeechSynthesisUtterance("Not recognized. Please repeat!");
			speech_synth.speak(utterThis);
	}
	
	recognition.start();
}
//----------------------------------------------------------------
function on_speech_no_match(event) 
{
	var utterThis = new SpeechSynthesisUtterance("Not recognized. Please repeat!");
	speech_synth.speak(utterThis);
}
//----------------------------------------------------------------
function on_speech_recognition_error(event) 
{
  diagnostic.textContent = 'Error occurred in recognition: ' + event.error;
}
//----------------------------------------------------------------
function on_speech_end()
{
	recognition.stop();
}
//----------------------------------------------------------------