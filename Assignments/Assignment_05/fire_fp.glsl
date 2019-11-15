#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 tex_coord;
in float step;

// Uniform (global) buffer
uniform sampler2D tex_samp;

// Simulation parameters (constants)
uniform vec3 object_color = vec3(0.8, 0.4, 0.03);


void main (void)
{
    // Get pixel from texture
    vec4 outval = texture(tex_samp, tex_coord);
    // Adjust specified object color according to the grayscale texture value

	vec3 c = vec3(outval.r*object_color.r, outval.g*object_color.g, outval.b*object_color.b);

    outval = vec4(step * c.rgb + (1-step)*c.bgr, sqrt(sqrt(outval.r))*frag_color.a / 2.0);

    // Set output fragment color
    gl_FragColor = outval;

}
