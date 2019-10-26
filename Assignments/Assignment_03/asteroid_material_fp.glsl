// Material with no illumination simulation

#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;


void main() 
{
	//gl_FragColor = color_interp;
	gl_FragColor = color_interp.x * vec4(0.0, 0.0, 1.0, 1.0) + (1.0-color_interp.x) * vec4(0.0, 1.0, 0.0, 1.0);
}
