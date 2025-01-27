// Material with no illumination simulation

#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;

// Uniform (global) buffer
uniform mat4 total_mat;

// Attributes forwarded to the fragment shader
out vec4 color_interp;


void main()
{
	gl_Position = total_mat * vec4(vertex, 1.0);

    color_interp = vec4(color, 1.0);
}
