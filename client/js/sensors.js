window.addEventListener("deviceorientation", on_device_orientation);
//-------------------------------------------------
function on_device_orientation(event) 
{
	var c_sensor_position = document.getElementById("c_sensor_position");
	
	var alpha = Math.round(event.alpha);
	var beta = Math.round(event.beta);
	var gamma = Math.round(event.gamma);
  
	if (alpha != null && beta != null){
		var center = {x : 200, y : 150};
		c_sensor_position.setAttribute("cx", center.x + alpha);
		c_sensor_position.setAttribute("cy", center.y + beta);
	}
}
//-------------------------------------------------