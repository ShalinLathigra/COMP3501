#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
//in float offset;
//in float outline;

//float width = 20.0;
//float speed = 250.0;
//float amplitude = .125;

void main()
{
	//Fun With Shaders
	//if (outline != 0)
	//{
	//	gl_FragColor = color_interp + (-2 * color_interp) * sin((gl_FragCoord.x - offset * speed) * amplitude / width);
	//}
	//else
	//{
		gl_FragColor = color_interp;
	//}
}