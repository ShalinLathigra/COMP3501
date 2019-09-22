#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform float z;
uniform float isColored;
//uniform float time;

// Attributes forwarded to the fragment shader
out vec4 color_interp;
//out float offset;
//out float outline;


void main()
{
	
	vec3 w = vec3(1.0, 1.0, 1.0);
	vec3 r = vec3(1.0, 0.0, 0.0);
	vec3 o = vec3(1.0, 0.5, 0.0);
	vec3 y = vec3(1.0, 1.0, 0.0);
	vec3 g = vec3(0.0, 1.0, 0.0);
	vec3 b = vec3(0.0, 0.0, 1.0);
	vec3 p = vec3(0.6, 0.0, 1.0);
	vec3 end;
	
	gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
	if (isColored == 0) {
		end = w;
	}
	else if (z > -1){
		end = (1 + z) * w + (-z) * r;
	} else if (z > -2){
		end = (2 + z) * r + (-z - 1) * o;
	} else if (z > -3){
		end = (3 + z) * o + (-z - 2) * y;
	} else if (z > -4){
		end = (4 + z) * y + (-z - 3) * g;
	} else if (z > -5){
		end = (5 + z) * g + (-z - 4) * b;
	} else if (z > -4){
		end = (6 + z) * b + (-z - 5) * p;
	} else {
	 	end = p;
	}
	
	end = (9 + z)/8.0 * end;
	color_interp = vec4(end, 1.0);
	//offset = time;
	//outline = isColored;
}