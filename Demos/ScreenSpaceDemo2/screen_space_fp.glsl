#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;

vec4 b_edge_color = vec4(1.0, 0.0, 0.0, 1.0);
vec4 f_edge_color = vec4(0.0, 1.0, 0.0, 1.0);
float edge_width = .05;
float speed = .5;

float overlay_tiling = 64.0;
float overlay_intensity = 2.0;
vec4 overlay_color = vec4(1.0, 1.0, 1.0, 1.0);

float pixel_compression = 8.0;
void main()
{
	// Do nothing (just copy input to output)
	/*
	vec2 pos = uv0;
	vec4 pixel = texture(texture_map, pos);
	gl_FragColor = pixel;
	*/

	//Fade-in/out from Center
	//Looping animation, I'd like to apply a bloom effect to the edge, but I think that'd need a second rendering pass.
	//Start from black, green ring expands and brings image back. A few seconds later a red ring erases the image.
	//Can modify edge_width, and the edge colors. b_edge_color = the erasing edge, f_edge_color = drawing edge.
	/*
	vec2 pos = uv0;
	vec4 pixel = texture(texture_map, pos);

	vec2 o = vec2(0.5, 0.5);
	float dist = 1.0 - (pow(o.x - uv0.x, 2) + pow(o.y - uv0.y, 2))* 2.0;

	float step = timer * speed;

	float mask = clamp(mod(int(dist + step), 4), 0, 1);
	float f_mask = clamp(mod(int(dist + step + edge_width), 2), 0, 1);
	float b_mask = clamp(mod(int(dist + step - edge_width), 2), 0, 1);
	float f_edge = clamp(f_mask - mask, 0, 1);
	float b_edge = clamp(b_mask - mask, 0, 1);

	vec4 fade_state = pixel * mask + f_edge_color * f_edge + b_edge_color * b_edge;
	gl_FragColor = fade_state;
	*/

	//Pixelate
	//Lowers resolution. 
	//Can modify based on pixel_compression value at top.
	/*
	int width = 800;	//width
	int height = 600;	//height

	//Will Assume resolution is constant for now.
	float x_compress = width / pixel_compression;
	float y_compress = height / pixel_compression;
	float x_coord = int(uv0.x * x_compress) / x_compress;
	float y_coord = int(uv0.y * y_compress) / y_compress;

	vec4 pixel = texture(texture_map, vec2(x_coord,y_coord));

	float c_dist = dot(pixel, vec4(0.0, 0.0, 1.0, 1.0));
	gl_FragColor = pixel;
	*/

	//Shields Online
	//Gives impression of shields being active. faint pattern glowing and flashing around the very fringes of the screen. Not intended to be incredibly distracting.
	//Modify by altering overlay_intensity, overlay_tiling, and overlay_color, to affect the brightness, density of the pattern, and color of the overlay.
	/*
	vec2 pos = uv0;
	vec2 offset = pos + vec2(sin(timer*.5), cos(timer*-.125))*.25;
	vec2 o = vec2(0.5, 0.5);

	float dist = (pow(o.x - uv0.x, 2) + pow(o.y - uv0.y, 2))*overlay_intensity;
	dist = pow(dist, 2);
	float step = max(sin(timer), 0)*.125 + .25;
	float map = abs(sin(offset.x * overlay_tiling) + cos(offset.y * overlay_tiling));
	float t = abs(dist * step * map);

	vec4 pixel = texture(texture_map, pos);

	gl_FragColor = pixel + t * overlay_color;
	*/
}
