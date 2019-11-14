#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;
uniform int effect_num = 0;

vec4 f_edge_color = vec4(0.005, 1.0, 0.005, 1.0);
float edge_width = .05;
float speed = .5;

float overlay_tiling = 64.0;
float overlay_intensity = 2.0;
vec4 overlay_color = vec4(1.0, 1.0, 1.0, 1.0);

float pixel_compression = 8.0;

/*
Random, Noise, Rotate2D functions found at: https://thebookofshaders.com/11/
*/

float random(in vec2 st) {
	return fract(sin(dot(st.xy,
		vec2(12.9898, 78.233)))
		* 43758.5453123);
}
float noise(in vec2 st) {
	vec2 i = floor(st);
	vec2 f = fract(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));

	// Smooth Interpolation

	// Cubic Hermine Curve.  Same as SmoothStep()
	vec2 u = smoothstep(0.,1.,f);

	// Mix 4 coorners percentages
	return mix(a, b, u.x) +
		(c - a)* u.y * (1.0 - u.x) +
		(d - b) * u.x * u.y;
}
mat2 rotate2d(float angle) {
	return mat2(cos(angle), -sin(angle),
		sin(angle), cos(angle));
}

void main()
{
	// Do nothing (just copy input to output)
	if (effect_num == 1)
	{
	vec2 pos = uv0;
	vec4 pixel = texture(texture_map, pos);
	gl_FragColor = pixel;
	}

	//Fade-in/out from Center
	//Looping animation, 
	else if (effect_num == 2)
	{
		vec2 pos = uv0;
		vec4 pixel = texture(texture_map, pos);

		vec2 o = vec2(0.5, 0.5);

		vec2 tra = pos - o;
		tra.x *= sin(timer * 2);
		tra.y *= cos(timer * 2);

		float dist = 1.0 - (pow(tra.x, 2) + pow(tra.y, 2))* 2.0;

		dist = max(sin(dist + timer), 0);

		float gb = max(dist - .5, 0);
		float gs = dist;
		float gd = min(dist + .5, 1);
			  
		float g_s = gd - gs;
		float g_b = gs - gb;

		float edge = pow((g_s + g_b), 10);

		gl_FragColor = edge * f_edge_color + clamp(2 * g_s, 0, 1) * pixel;

	}
	//gl_FragColor = vec4(g_s, 0.0, g_b, 1.0);

	//Noise, Outlines only, Everything is red.
	//Intentionally imperfect, supposed to give impression of things having gone horribly wrong.
	//Torus is turned red, appears slightly pixelated, screen is covered in red static.
	else if (effect_num == 3)
	{
		int width = 800;	//width
		int height = 600;	//height

		//Will Assume resolution is constant for now.
		float x_compress = width / pixel_compression;
		float y_compress = height / pixel_compression;
		float x_coord = int(uv0.x * x_compress) / x_compress;
		float y_coord = int(uv0.y * y_compress) / y_compress;

		vec4 pixel = texture(texture_map, vec2(x_coord, y_coord));

		//float c_dist = dot(pixel, vec4(0.0, 0.0, 1.0, 1.0));
		gl_FragColor = pixel;

		vec2 pos = uv0;
		
		float xm = pos.x - 0.005;
		float xp = pos.x + 0.005;
		float ym = pos.y - 0.005;
		float yp = pos.y + 0.005;

		vec4 hp1 = texture(texture_map, vec2(xm, ym));
		vec4 hp2 = texture(texture_map, vec2(xp, ym));
		vec4 hp3 = texture(texture_map, vec2(xm, pos.y));
		vec4 hp4 = texture(texture_map, vec2(xp, pos.y));
		vec4 hp5 = texture(texture_map, vec2(xm, yp));
		vec4 hp6 = texture(texture_map, vec2(xp, yp));
		vec4 hdiff = hp1 - hp2 + 2.0*hp3 - 2.0*hp4 + hp5 - hp6;


		vec4 vp1 = texture(texture_map, vec2(xm, ym));
		vec4 vp2 = texture(texture_map, vec2(xm, yp));
		vec4 vp3 = texture(texture_map, vec2(pos.x, ym));
		vec4 vp4 = texture(texture_map, vec2(pos.x, yp));
		vec4 vp5 = texture(texture_map, vec2(xp, ym));
		vec4 vp6 = texture(texture_map, vec2(xp, yp));
		vec4 vdiff = hp1 - hp2 + 2.0*hp3 - 2.0*hp4 + hp5 - hp6;

		float d = pow(hdiff.z, 2) + pow(vdiff.z, 2);

		gl_FragColor = pixel * vec4(d, d, d, 1);

		//Need to add kind of static + lines
		float static_map = noise((uv0 + vec2(sin(timer * 25) * 12, cos(timer*-35) * 7)) * 256);
		float brightness_map = noise((uv0 + vec2(sin(timer * -10) * 4, cos(timer*3) * 2)) * 10);
		float line_map = cos((pos.x + pixel.b) * 50.0 - timer * 3.0) * mod(pos.y * 10, 1) * mod(pos.x * 10 + timer, 1) + 0.75;

		gl_FragColor = clamp(line_map * brightness_map, .2, .5) * static_map * vec4(0.7, 0.1, 0.2, 1.0) + pixel.bgra * vec4(d, d, d, 1);
	}
	//Woozy
	//Psychadelic pattern in shifting colours around the edges, pulsating a bit to make it more noticeable.
	//Screen rotates back and forth a bit to make it a bit worse. Screen will flash about once a second
	else if (effect_num == 4)
	{
		vec2 pos = uv0;
		vec2 o = vec2(0.5, 0.5);

		float dist = (pow(o.x - uv0.x, 2) + pow(o.y - uv0.y, 2))*overlay_intensity;
		dist = pow(dist, 2);
		float step = max(sin(timer), 0)*.125 + .25;

		//Create swirling around edges of screen
		vec2 offset = rotate2d(noise(pos*sin(timer / 4.0)))*pos + vec2(sin(timer*.5), cos(timer*-.125))*.25;
		offset *= 16;
		offset.x = sin(sin(offset.x) + cos(offset.y));
		offset.y = cos(sin(offset.x*offset.y) + cos(offset.x));
		float swirl_map = abs(sin(offset.x * overlay_tiling) + cos(offset.y * overlay_tiling));

		float t = abs(dist * step * swirl_map);

		vec2 tra = pos - o;
		tra.x *= sin(timer * 2);
		tra.y *= cos(timer * 2);

		float brightness = 1.0 - (pow(tra.x, 2) + pow(tra.y, 2))*2.0;


		//Add slight swirl effect to screen
		vec2 tex_off = (rotate2d(sin(timer) * cos(timer) * .125) *(pos - vec2(0.5))) + vec2(0.5);
		vec4 pixel = texture(texture_map, tex_off);

		gl_FragColor = brightness * pixel + t * vec4(abs(sin(timer + .5)), abs(cos(-timer)), sin(0.9 + timer) * 5.0 - 4.0, 1.0);
	}
}
