// Author: Mihai Oltean, https://mihaioltean.github.io, mihai.oltean@gmail.com
// More details: https://jenny5.org, https://jenny5-robot.github.io/
// Source code: github.com/jenny5-robot
// License: MIT
//--------------------------------------------
"use strict";
window.addEventListener("deviceorientation", on_device_orientation);

var previous_gamma = 0;
var previous_beta = 0;

//-------------------------------------------------
function on_device_orientation(event) 
{
	var c_sensor_position = document.getElementById("c_sensor_position");
	
	if (event.beta != null){// make no sense to continue if I don't have access to sensors
		var alpha = Math.round(event.alpha);
		var beta = Math.round(event.beta);
		var gamma = Math.round(event.gamma);
	  
		if (alpha != null && beta != null && gamma != null){
			
			c_sensor_position.setAttribute("cx", sensor_bbox_center.x + gamma * sensor_bbox_width / 180);
			c_sensor_position.setAttribute("cy", sensor_bbox_center.y + beta * sensor_bbox_height / 180);
			
			if (beta > -90 && beta < 90)
				if (Math.abs(previous_gamma - gamma) > 5 || Math.abs(previous_beta - beta) > 5){
					var bx = (gamma / 2.9 + 32);
					var by = (beta / 2.9 + 32);

					var b = new Uint8Array(2);
					b[0] = bx;
					b[1] = by;
				
					b[0] |= (1 << 7);
					b[1] |= (1 << 6);
				
					send_command_to_robot(b[0], b[1]);
					//console.log(gamma + " " + beta + " " + b[0] + " " + b[1]);
				
					previous_gamma = gamma;
					previous_beta = beta;
				}
		}
	}
}
//-------------------------------------------------