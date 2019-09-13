#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Attributes forwarded to the fragment shader
out vec4 color_interp;


void main()
{
	vec3 r = vec3(1.0, 0.0, 0.0);
	vec3 o = vec3(1.0, 0.5, 0.0);
	vec3 y = vec3(1.0, 1.0, 0.0);
	vec3 g = vec3(0.0, 1.0, 0.0);
	vec3 b = vec3(0.0, 0.0, 1.0);
	vec3 p = vec3(1.0, 0.0, 1.0);
	
	float rT = 1.0;
	float oT = 2.0;
	float yT = 3.0;
	float gT = 4.0;
	float bT = 5.0;
	float pT = 6.0;

    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

	float z = -1.0 * vertex.z;

	if (z<3)
	{
		color_interp = vec4(color, 1.0);
	} 
	else
	{
		color_interp = vec4(r, 1.0);
	}
	/*
	else if (z < rT)
	{
		color_interp = vec4((rT - z) * color + z * r, 1.0);
	} 
	else if (z < oT)
	{
		color_interp = vec4((oT - z) * r + z * o, 1.0);
	} 
	else if (z < yT)
	{
		color_interp = vec4((yT - z) * b + z * y, 1.0);
	} 
	else if (z < gT)
	{
		color_interp = vec4((gT - z) * y + z * g, 1.0);
	} 
	else if (z < bT)
	{
		color_interp = vec4((bT - z) * g + z * b, 1.0);
	} 
	else if (z < pT)
	{
		color_interp = vec4((pT - z) * b + z * p, 1.0);
	}
	else
	{
		color_interp = vec4(p, 1.0);
	}*/
}
