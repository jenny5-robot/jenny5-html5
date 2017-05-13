var arrayBuffer;
var websocket;
//--------------------------------------------------------------------
function ws_connect()
{
			var addr = "wss://" + window.location.host;
		alert(addr);
	var button_connect = document.getElementById("connect_button");
	
	if (button_connect.innerText == "Connect"){
		var addr = "wss://" + window.location.host;
		alert(addr);
        websocket = new WebSocket(addr, "data");
		websocket.onopen = ws_on_open;
		websocket.onerror = ws_on_error;
		websocket.onmessage = ws_on_message_received;
		websocket.onclose = ws_on_close;
	}
	else{
		websocket.close(1000, "normal shutdown");
	}
}
//--------------------------------------------------------------------
function ws_on_open() 
{
	set_disable_state(false);
	document.getElementById("connect_button").innerText = "Disconnect";
}
//--------------------------------------------------------------------
function ws_on_error() 
{
	alert("websocket error");
}
//--------------------------------------------------------------------
function ws_on_close()
{
	document.getElementById("connect_button").innerText = "Connect";
	set_disable_state(true);
}
//--------------------------------------------------------------------
function ws_on_message_received_parsed(e)
{
	alert(arrayBuffer);
}
//--------------------------------------------------------------------
function ws_on_message_received(evt) 
{
	var fileReader = new FileReader();
	fileReader.onload = function() {arrayBuffer = this.result;};
	fileReader.addEventListener("loadend", ws_on_message_received_parsed);
	fileReader.readAsText(evt.data);

    console.log(evt.data);
}
//--------------------------------------------------------------------
function send_message_to_server()
{
	websocket.send("Hello Jenny 5!");
}
//--------------------------------------------------------------------