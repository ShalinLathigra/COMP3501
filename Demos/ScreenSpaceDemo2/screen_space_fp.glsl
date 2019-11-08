#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;

vec4 f_edge_color = vec4(1.0, 0.0, 0.0, 1.0);
vec4 b_edge_color = vec4(0.0, 1.0, 0.0, 1.0);
float edge_width = .05;
float speed = .5;

void main()
{
	// Do nothing (just copy input to output)
	
	vec2 pos = uv0;
	vec4 pixel = texture(texture_map, pos);
	gl_FragColor = pixel;
	

	//Fade-in/out from Center
	/*
	vec2 o = vec2(0.5, 0.5);
	float dist = 1.0 - (pow(o.x - uv0.x, 2) + pow(o.y - uv0.y, 2))* 2.0;

	float step = timer * speed;

	float mask = clamp(mod(int(dist + step), 4), 0, 1);
	float f_mask = clamp(mod(int(dist + step + edge_width), 2), 0, 1);
	float b_mask = clamp(mod(int(dist + step - edge_width), 2), 0, 1);
	float f_edge = f_mask - mask;
	float b_edge = b_mask - mask;

	vec4 fade_state = pixel * mask + f_edge_color * f_edge + b_edge_color * b_edge;
	gl_FragColor = fade_state;
	*/

	//Pixelate
	/*
	int compression = 8;	//convert 2x2s into single pixel values
	int width = 800;	//width
	int height = 600;	//height

	//Will Assume resolution is constant for now.
	float x_compress = width / compression;
	float y_compress = height / compression;
	float x_coord = int(uv0.x * x_compress) / x_compress;
	float y_coord = int(uv0.y * y_compress) / y_compress;

	vec4 pixel = texture(texture_map, vec2(x_coord,y_coord));

	float c_dist = dot(pixel, vec4(0.0, 0.0, 1.0, 1.0));
	gl_FragColor = pixel;
	*/

	/*
	Screen stretches towards sides, pinches near middle
	lightning in screen
	*/
}
