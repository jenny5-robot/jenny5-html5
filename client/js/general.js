
//----------------------------------------------------------------
function set_disable_state(state)
{
	document.getElementById("send_message_button").disabled = state;
	document.getElementById("head_camera_button").disabled = state;
	document.getElementById("left_arm_camera_button").disabled = state;
}
//----------------------------------------------------------------
function on_load_page()
{
	set_disable_state(true);
}
//----------------------------------------------------------------